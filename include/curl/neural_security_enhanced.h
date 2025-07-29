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

#ifndef CURL_NEURAL_SECURITY_ENHANCED_H
#define CURL_NEURAL_SECURITY_ENHANCED_H

#include "curl_setup.h"
#include <curl/curl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Neural Security Network */
typedef struct neural_security_network neural_security_network_t;

/* Biometric Authentication */
typedef struct biometric_auth biometric_auth_t;

/* Zero-Trust Architecture */
typedef struct zero_trust_context zero_trust_context_t;

/* AI-Powered Threat Detection */
typedef struct threat_detection threat_detection_t;

/* Quantum Encryption */
typedef struct quantum_encryption quantum_encryption_t;

/* Neural Security Core Functions */
void neural_security_system_init(void);
void neural_security_system_cleanup(void);

/* Neural Security Validation */
CURLcode curl_neural_security_validate(const char* data, const char* context, char** security_report);

/* Threat Detection */
threat_detection_t* ai_threat_detect(const char* input_data, void* network);

/* Biometric Authentication */
biometric_auth_t* biometric_auth_create(const char* user_id);

/* Zero-Trust Functions */
zero_trust_context_t* zero_trust_create(const char* device_id, const char* location);

/* Quantum Encryption */
quantum_encryption_t* quantum_encryption_create(void);

/* Self-Healing Security */
void self_healing_security_update(void* network);

#ifdef __cplusplus
}
#endif

#endif /* CURL_NEURAL_SECURITY_ENHANCED_H */
