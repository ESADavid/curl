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

#ifndef CURL_PERFECT_AI_INTEGRATION_H
#define CURL_PERFECT_AI_INTEGRATION_H

#include "curl_setup.h"
#include <curl/curl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Perfect AI Response */
typedef struct perfect_ai_response perfect_ai_response_t;

/* Perfect AI State */
typedef struct perfect_ai_state perfect_ai_state_t;

/* Perfect AI Core Functions */
void perfect_ai_system_init(void);
void perfect_ai_system_cleanup(void);

/* Perfect AI Validation */
CURLcode curl_perfect_ai_validate(const char* validation_data, const char* context, const char* emotional_state, perfect_ai_response_t** response);

/* Perfect AI Status */
char* perfect_ai_get_status(void);

/* Perfect AI Demonstration */
void perfect_ai_demonstration(void);

/* Response Management */
void perfect_ai_response_destroy(perfect_ai_response_t* response);

#ifdef __cplusplus
}
#endif

#endif /* CURL_PERFECT_AI_INTEGRATION_H */
