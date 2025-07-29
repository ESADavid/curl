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

#ifndef CURL_VALIDATION_ENHANCED_H
#define CURL_VALIDATION_ENHANCED_H

#include "curl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Enhanced error handling */
typedef struct {
    int code;
    char* message;
    char* details;
    char* timestamp;
    char* request_id;
} validation_error_t;

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

/* Performance metrics */
typedef struct {
    double request_time;
    int retry_count;
    size_t response_size;
    time_t timestamp;
} validation_metrics_t;

/* Enhanced API functions */
CURLcode curl_validation_enhanced(CURL* curl, const char* endpoint, const char* payload, validation_error_t* error);
void validation_set_config(const validation_config_t* config);
void validation_cleanup(void);
int validation_get_metrics(validation_metrics_t** metrics, int* count);

/* Configuration helpers */
void validation_set_base_url(const char* url);
void validation_set_timeout(int seconds);
void validation_set_max_retries(int retries);
void validation_enable_caching(bool enable);
void validation_enable_metrics(bool enable);
void validation_enable_connection_pooling(bool enable);
void validation_set_client_cert(const char* cert_path, const char* key_path);

/* Utility functions */
void validation_free_error(validation_error_t* error);
void validation_free_metrics(validation_metrics_t* metrics, int count);

#ifdef __cplusplus
}
#endif

#endif /* CURL_VALIDATION_ENHANCED_H */
