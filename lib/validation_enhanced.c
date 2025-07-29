/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 2024, JPMorgan Chase & Co.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/

#include "curl_setup.h"
#include "curl/validation.h"
#include "curl/validation_payroll.h"
#include "validation_json.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* Enhanced error handling */
typedef struct {
    int code;
    char* message;
    char* details;
    char* timestamp;
    char* request_id;
} validation_error_t;

/* Connection pooling */
typedef struct {
    CURL** connections;
    int pool_size;
    int current_index;
    int max_connections;
} connection_pool_t;

/* Response caching */
typedef struct {
    char* key;
    char* response;
    time_t expiry;
    struct cache_entry* next;
} cache_entry_t;

/* Performance metrics */
typedef struct {
    double request_time;
    int retry_count;
    size_t response_size;
    time_t timestamp;
} validation_metrics_t;

/* Enhanced configuration */
typedef struct {
    char* base_url;
    int timeout;
    int max_retries;
    bool enable_caching;
    bool enable_metrics;
    bool enable_connection_pooling;
    char* client_cert_path;
    char* client_key_path;
} validation_config_t;

/* Global configuration */
static validation_config_t global_config = {
    .base_url = "https://api-mock.payments.jpmorgan.com/tsapi/v2",
    .timeout = 30,
    .max_retries = 3,
    .enable_caching = true,
    .enable_metrics = true,
    .enable_connection_pooling = true,
    .client_cert_path = NULL,
    .client_key_path = NULL
};

/* Connection pool */
static connection_pool_t connection_pool = {
    .connections = NULL,
    .pool_size = 0,
    .current_index = 0,
    .max_connections = 10
};

/* Cache */
static cache_entry_t* cache_head = NULL;
static const int CACHE_TTL = 300; // 5 minutes

/* Metrics */
static validation_metrics_t* metrics_buffer = NULL;
static int metrics_count = 0;
static const int MAX_METRICS = 1000;

/* Enhanced error handling */
static void set_validation_error(validation_error_t* error, int code, const char* message, const char* details) {
    if (!error) return;
    
    error->code = code;
    error->message = strdup(message);
    error->details = details ? strdup(details) : NULL;
    
    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    error->timestamp = strdup(time_str);
}

/* Connection pool management */
static CURL* get_connection_from_pool(void) {
    if (!global_config.enable_connection_pooling || connection_pool.pool_size == 0) {
        return curl_easy_init();
    }
    
    CURL* curl = connection_pool.connections[connection_pool.current_index];
    connection_pool.current_index = (connection_pool.current_index + 1) % connection_pool.pool_size;
    return curl;
}

static void return_connection_to_pool(CURL* curl) {
    if (!global_config.enable_connection_pooling) {
        curl_easy_cleanup(curl);
        return;
    }
    
    if (connection_pool.pool_size < connection_pool.max_connections) {
        if (!connection_pool.connections) {
            connection_pool.connections = malloc(sizeof(CURL*) * connection_pool.max_connections);
        }
        connection_pool.connections[connection_pool.pool_size++] = curl;
    } else {
        curl_easy_cleanup(curl);
    }
}

/* Caching */
static char* get_cached_response(const char* key) {
    if (!global_config.enable_caching) return NULL;
    
    time_t now = time(NULL);
    cache_entry_t* current = cache_head;
    cache_entry_t* prev = NULL;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (current->expiry > now) {
                // Move to front (LRU)
                if (prev) {
                    prev->next = current->next;
                    current->next = cache_head;
                    cache_head = current;
                }
                return strdup(current->response);
            } else {
                // Remove expired entry
                if (prev) {
                    prev->next = current->next;
                } else {
                    cache_head = current->next;
                }
                free(current->key);
                free(current->response);
                free(current);
                return NULL;
            }
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

static void cache_response(const char* key, const char* response) {
    if (!global_config.enable_caching) return;
    
    // Remove oldest if cache is full
    int count = 0;
    cache_entry_t* current = cache_head;
    while (current && count < 100) {
        count++;
        current = current->next;
    }
    
    if (count >= 100) {
        // Remove last entry
        cache_entry_t* prev = NULL;
        current = cache_head;
        while (current && current->next) {
            prev = current;
            current = current->next;
        }
        if (prev) {
            prev->next = NULL;
            free(current->key);
            free(current->response);
            free(current);
        }
    }
    
    // Add new entry
    cache_entry_t* entry = malloc(sizeof(cache_entry_t));
    entry->key = strdup(key);
    entry->response = strdup(response);
    entry->expiry = time(NULL) + CACHE_TTL;
    entry->next = cache_head;
    cache_head = entry;
}

/* Enhanced retry logic */
static CURLcode validation_retry_with_backoff(CURL* curl, int max_retries) {
    CURLcode res;
    int retry_count = 0;
    int backoff_ms = 1000; // Start with 1 second
    
    while (retry_count < max_retries) {
        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            
            if (response_code >= 200 && response_code < 300) {
                return res;
            }
            
            // Retry on 5xx errors
            if (response_code >= 500) {
                retry_count++;
                if (retry_count < max_retries) {
                    sleep(backoff_ms / 1000);
                    backoff_ms *= 2; // Exponential backoff
                }
                continue;
            }
        }
        
        // Retry on network errors
        if (res == CURLE_COULDNT_CONNECT || res == CURLE_OPERATION_TIMEDOUT) {
            retry_count++;
            if (retry_count < max_retries) {
                sleep(backoff_ms / 1000);
                backoff_ms *= 2;
                continue;
            }
        }
        
        break;
    }
    
    return res;
}

/* Enhanced metrics collection */
static void record_metrics(validation_metrics_t* metrics) {
    if (!global_config.enable_metrics) return;
    
    if (!metrics_buffer) {
        metrics_buffer = malloc(sizeof(validation_metrics_t) * MAX_METRICS);
    }
    
    if (metrics_count < MAX_METRICS) {
        metrics_buffer[metrics_count++] = *metrics;
    }
}

/* Enhanced configuration management */
void validation_set_config(const validation_config_t* config) {
    if (config->base_url) {
        free(global_config.base_url);
        global_config.base_url = strdup(config->base_url);
    }
    global_config.timeout = config->timeout;
    global_config.max_retries = config->max_retries;
    global_config.enable_caching = config->enable_caching;
    global_config.enable_metrics = config->enable_metrics;
    global_config.enable_connection_pooling = config->enable_connection_pooling;
    
    if (config->client_cert_path) {
        free(global_config.client_cert_path);
        global_config.client_cert_path = strdup(config->client_cert_path);
    }
    
    if (config->client_key_path) {
        free(global_config.client_key_path);
        global_config.client_key_path = strdup(config->client_key_path);
    }
}

/* Enhanced validation with all improvements */
CURLcode curl_validation_enhanced(CURL* curl, const char* endpoint, const char* payload, validation_error_t* error) {
    if (!curl || !endpoint || !payload) {
        if (error) {
            set_validation_error(error, 400, "Invalid parameters", "curl, endpoint, or payload is NULL");
        }
        return CURLE_BAD_FUNCTION_ARGUMENT;
    }
    
    // Create cache key
    char cache_key[256];
    snprintf(cache_key, sizeof(cache_key), "%s:%s", endpoint, payload);
    
    // Check cache
    char* cached_response = get_cached_response(cache_key);
    if (cached_response) {
        free(cached_response);
        return CURLE_OK;
    }
    
    // Get connection from pool
    CURL* connection = get_connection_from_pool();
    if (!connection) {
        if (error) {
            set_validation_error(error, 500, "Connection pool exhausted", "Unable to get connection from pool");
        }
        return CURLE_OUT_OF_MEMORY;
    }
    
    // Setup request
    struct curl_slist* headers = NULL;
    char url[512];
    snprintf(url, sizeof(url), "%s/%s", global_config.base_url, endpoint);
    
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    
    curl_easy_setopt(connection, CURLOPT_URL, url);
    curl_easy_setopt(connection, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(connection, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(connection, CURLOPT_TIMEOUT, global_config.timeout);
    
    // Add client certificate if configured
    if (global_config.client_cert_path) {
        curl_easy_setopt(connection, CURLOPT_SSLCERT, global_config.client_cert_path);
    }
    if (global_config.client_key_path) {
        curl_easy_setopt(connection, CURLOPT_SSLKEY, global_config.client_key_path);
    }
    
    // Record metrics
    validation_metrics_t metrics = {0};
    time_t start_time = time(NULL);
    
    // Perform request with retry
    CURLcode res = validation_retry_with_backoff(connection, global_config.max_retries);
    
    // Record metrics
    metrics.request_time = difftime(time(NULL), start_time);
    metrics.retry_count = 0; // Would be tracked in retry function
    record_metrics(&metrics);
    
    // Cache successful responses
    if (res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(connection, CURLINFO_RESPONSE_CODE, &response_code);
        
        if (response_code >= 200 && response_code < 300) {
            cache_response(cache_key, "success");
        } else {
            if (error) {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "HTTP %ld", response_code);
                set_validation_error(error, (int)response_code, error_msg, "API returned non-200 status");
            }
        }
    } else {
        if (error) {
            set_validation_error(error, (int)res, curl_easy_strerror(res), "CURL operation failed");
        }
    }
    
    // Cleanup
    curl_slist_free_all(headers);
    return_connection_to_pool(connection);
    
    return res;
}

/* Cleanup function */
void validation_cleanup(void) {
    // Cleanup cache
    cache_entry_t* current = cache_head;
    while (current) {
        cache_entry_t* next = current->next;
        free(current->key);
        free(current->response);
        free(current);
        current = next;
    }
    cache_head = NULL;
    
    // Cleanup connection pool
    for (int i = 0; i < connection_pool.pool_size; i++) {
        curl_easy_cleanup(connection_pool.connections[i]);
    }
    free(connection_pool.connections);
    connection_pool.connections = NULL;
    connection_pool.pool_size = 0;
    
    // Cleanup metrics
    free(metrics_buffer);
    metrics_buffer = NULL;
    metrics_count = 0;
    
    // Cleanup configuration
    free(global_config.base_url);
    free(global_config.client_cert_path);
    free(global_config.client_key_path);
}

/* Get metrics summary */
int validation_get_metrics(validation_metrics_t** metrics, int* count) {
    if (!global_config.enable_metrics) {
        *metrics = NULL;
        *count = 0;
        return 0;
    }
    
    *metrics = malloc(sizeof(validation_metrics_t) * metrics_count);
    if (!*metrics) return -1;
    
    memcpy(*metrics, metrics_buffer, sizeof(validation_metrics_t) * metrics_count);
    *count = metrics_count;
    
    return 0;
}
