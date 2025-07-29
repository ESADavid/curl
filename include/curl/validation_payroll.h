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

#ifndef CURL_VALIDATION_PAYROLL_H
#define CURL_VALIDATION_PAYROLL_H

#include "curl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Payroll validation request structure */
struct payroll_validation_request {
  char *request_id;
  char *account_number;
  char *financial_institution_id;
  char *clearing_system_id;
  char *clearing_system_id_type;
  char *entity_type;
  char *first_name;
  char *last_name;
  char *full_name;
  char *payroll_amount;
  char *payroll_currency;
  char *payroll_frequency;
  char *employer_name;
  char *employer_id;
};

/* Payroll validation API functions */
CURLcode curl_validation_payroll(CURL *curl, const struct payroll_validation_request *request);
char *validation_create_payroll_payload(const struct payroll_validation_request *request);
void validation_free_payroll_payload(char *payload);

#ifdef __cplusplus
}
#endif

#endif /* CURL_VALIDATION_PAYROLL_H */
