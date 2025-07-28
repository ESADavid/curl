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
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/

#ifndef CURL_VALIDATION_H
#define CURL_VALIDATION_H

#include "curl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Validation API options */
typedef enum {
  CURLOPT_VALIDATION_CLIENT_ID = 10000,
  CURLOPT_VALIDATION_PROGRAM_ID = 10001,
  CURLOPT_VALIDATION_PROGRAM_ID_TYPE = 10002,
  CURLOPT_VALIDATION_ENDPOINT = 10003,
  CURLOPT_VALIDATION_PAYLOAD = 10004
} CURLvalidationoption;

/* Validation request types */
typedef enum {
  VALIDATION_TYPE_ACCOUNT = 1,
  VALIDATION_TYPE_ENTITY = 2
} validation_type_t;

/* Entity types */
typedef enum {
  ENTITY_TYPE_INDIVIDUAL = 1,
  ENTITY_TYPE_ORGANIZATION = 2
} entity_type_t;

/* Validation response structure */
struct validation_response {
  char *response_data;
  size_t response_size;
  int http_status;
  char *error_message;
};

/* Account validation request structure */
struct account_validation_request {
  char *account_number;
  char *financial_institution_id;
  char *clearing_system_id;
  char *clearing_system_id_type;
  char *entity_type;
  char *request_id;
};

/* Entity validation request structure */
struct entity_validation_request {
  char *entity_type;
  char *first_name;
  char *last_name;
  char *full_name;
  char *postal_address;
  char *identification;
  char *request_id;
};

/* Validation context structure */
struct validation_context {
  char *client_id;
  char *program_id;
  char *program_id_type;
  char *endpoint_url;
  char *payload;
  size_t payload_len;
};

/* Core validation functions */
CURLcode curl_easy_setopt_validation(CURL *curl, CURLoption option, ...);
CURLcode curl_easy_perform_validation(CURL *curl);
char *curl_easy_getinfo_validation_response(CURL *curl);
void curl_easy_cleanup_validation(CURL *curl);

/* Helper functions */
CURLcode curl_validation_account(CURL *curl, const struct account_validation_request *request);
CURLcode curl_validation_entity(CURL *curl, const struct entity_validation_request *request);

/* JSON utilities */
char *validation_create_account_payload(const struct account_validation_request *request);
char *validation_create_entity_payload(const struct entity_validation_request *request);
void validation_free_payload(char *payload);

/* Error handling */
const char *validation_strerror(CURLcode code);

#ifdef __cplusplus
}
#endif

#endif /* CURL_VALIDATION_H */
