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

#include "curl/curl.h"
#include "curl_setup.h"
#include "validation.h"
#include "curl_memory.h"
#include "dynbuf.h"
#include "strcase.h"
#include "strdup.h"
#include <string.h>

/* The last #include file should be: */
#include "memdebug.h"

/* CEO Payroll validation request structure */
struct payroll_ceo_validation_request {
  char *request_id;
  char *account_number;
  char *financial_institution_id;
  char *clearing_system_id;
  char *clearing_system_id_type;
  char *first_name;
  char *last_name;
  char *full_name;
  char *payroll_amount;
  char *payroll_currency;
  char *employer_name;
  char *employer_id;
  char *ceo_title;
  char *ceo_department;
};

/* JSON string escaping helper */
static char *json_escape_string(const char *str)
{
  struct dynbuf buf;
  const unsigned char *p = (const unsigned char *)str;
  
  if(!str)
    return strdup("");

  Curl_dyn_init(&buf, 1024);
  
  while(*p) {
    switch(*p) {
    case '"':
      Curl_dyn_addn(&buf, "\\\"", 2);
      break;
    case '\\':
      Curl_dyn_addn(&buf, "\\\\", 2);
      break;
    case '\b':
      Curl_dyn_addn(&buf, "\\b", 2);
      break;
    case '\f':
      Curl_dyn_addn(&buf, "\\f", 2);
      break;
    case '\n':
      Curl_dyn_addn(&buf, "\\n", 2);
      break;
    case '\r':
      Curl_dyn_addn(&buf, "\\r", 2);
      break;
    case '\t':
      Curl_dyn_addn(&buf, "\\t", 2);
      break;
    default:
      if(*p < 32) {
        char tmp[7];
        msnprintf(tmp, sizeof(tmp), "\\u%04x", *p);
        Curl_dyn_add(&buf, tmp);
      } else {
        Curl_dyn_addn(&buf, p, 1);
      }
      break;
    }
    p++;
  }
  
  return Curl_dyn_ptr(&buf);
}

/* Create JSON payload for CEO payroll validation */
char *validation_create_payroll_ceo_payload(const struct payroll_ceo_validation_request *request)
{
  struct dynbuf buf;
  char *escaped_account = NULL;
  char *escaped_financial_id = NULL;
  char *escaped_clearing_id = NULL;
  char *escaped_clearing_type = NULL;
  char *escaped_first_name = NULL;
  char *escaped_last_name = NULL;
  char *escaped_full_name = NULL;
  char *escaped_amount = NULL;
  char *escaped_currency = NULL;
  char *escaped_employer = NULL;
  char *escaped_employer_id = NULL;
  char *escaped_request_id = NULL;
  char *escaped_ceo_title = NULL;
  char *escaped_ceo_department = NULL;

  if(!request)
    return NULL;

  Curl_dyn_init(&buf, 4096);

  escaped_account = json_escape_string(request->account_number);
  escaped_financial_id = json_escape_string(request->financial_institution_id);
  escaped_clearing_id = json_escape_string(request->clearing_system_id);
  escaped_clearing_type = json_escape_string(request->clearing_system_id_type);
  escaped_first_name = json_escape_string(request->first_name);
  escaped_last_name = json_escape_string(request->last_name);
  escaped_full_name = json_escape_string(request->full_name);
  escaped_amount = json_escape_string(request->payroll_amount);
  escaped_currency = json_escape_string(request->payroll_currency);
  escaped_employer = json_escape_string(request->employer_name);
  escaped_employer_id = json_escape_string(request->employer_id);
  escaped_request_id = json_escape_string(request->request_id);
  escaped_ceo_title = json_escape_string(request->ceo_title);
  escaped_ceo_department = json_escape_string(request->ceo_department);

  Curl_dyn_add(&buf, "[\n");
  Curl_dyn_add(&buf, "  {\n");
  Curl_dyn_add(&buf, "    \"requestId\": \"");
  Curl_dyn_add(&buf, escaped_request_id ? escaped_request_id : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "    \"account\": {\n");
  Curl_dyn_add(&buf, "      \"accountNumber\": \"");
  Curl_dyn_add(&buf, escaped_account ? escaped_account : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "      \"financialInstitutionId\": {\n");
  Curl_dyn_add(&buf, "        \"clearingSystemId\": {\n");
  Curl_dyn_add(&buf, "          \"id\": \"");
  Curl_dyn_add(&buf, escaped_clearing_id ? escaped_clearing_id : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "          \"idType\": \"");
  Curl_dyn_add(&buf, escaped_clearing_type ? escaped_clearing_type : "");
  Curl_dyn_add(&buf, "\"\n");
  Curl_dyn_add(&buf, "        }\n");
  Curl_dyn_add(&buf, "      }\n");
  Curl_dyn_add(&buf, "    },\n");
  Curl_dyn_add(&buf, "    \"entity\": {\n");
  Curl_dyn_add(&buf, "      \"individual\": {\n");
  Curl_dyn_add(&buf, "        \"firstName\": \"");
  Curl_dyn_add(&buf, escaped_first_name ? escaped_first_name : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "        \"lastName\": \"");
  Curl_dyn_add(&buf, escaped_last_name ? escaped_last_name : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "        \"fullName\": \"");
  Curl_dyn_add(&buf, escaped_full_name ? escaped_full_name : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "        \"title\": \"");
  Curl_dyn_add(&buf, escaped_ceo_title ? escaped_ceo_title : "");
  Curl_dyn_add(&buf, "\",\n");
  Curl_dyn_add(&buf, "        \"department\": \"");
  Curl_dyn_add(&buf, escaped_ceo_department ? escaped_ceo_department : "");
  Curl_dyn_add(&buf, "\"\n");
  Curl_dyn_add(&buf, "      }\n");
  Curl_dyn_add(&buf, "    },\n");
  Curl_dyn_add(&buf, "    \"transactions\": [\n");
  Curl_dyn_add(&buf, "      {\n");
  Curl_dyn_add(&buf, "        \"context\": \"PAYROLL\",\n");
  Curl_dyn_add(&buf, "        \"amount\": {\n");
  Curl_dyn_add(&buf, "          \"amount\": ");
  Curl_dyn_add(&buf, escaped_amount ? escaped_amount : "0.00");
  Curl_dyn_add(&buf, ",\n");
  Curl_dyn_add(&buf, "          \"currency\": \"");
  Curl_dyn_add(&buf, escaped_currency ? escaped_currency : "USD");
  Curl_dyn_add(&buf, "\"\n");
  Curl_dyn_add(&buf, "        }\n");
  Curl_dyn_add(&buf, "      }\n");
  Curl_dyn_add(&buf, "    ]\n");
  Curl_dyn_add(&buf, "  }\n");
  Curl_dyn_add(&buf, "]");

  /* Free escaped strings */
  free(escaped_account);
  free(escaped_financial_id);
  free(escaped_clearing_id);
  free(escaped_clearing_type);
  free(escaped_first_name);
  free(escaped_last_name);
  free(escaped_full_name);
  free(escaped_amount);
  free(escaped_currency);
  free(escaped_employer);
  free(escaped_employer_id);
  free(escaped_request_id);
  free(escaped_ceo_title);
  free(escaped_ceo_department);

  return Curl_dyn_ptr(&buf);
}

/* CEO Payroll validation API function */
CURLcode curl_validation_payroll_ceo(CURL *curl, const struct payroll_ceo_validation_request *request)
{
  char *payload = NULL;
  CURLcode result = CURLE_OK;

  if(!curl || !request)
    return CURLE_BAD_FUNCTION_ARGUMENT;

  payload = validation_create_payroll_ceo_payload(request);
  if(!payload)
    return CURLE_OUT_OF_MEMORY;

  /* Configure for payroll validation */
  curl_easy_setopt(curl, CURLOPT_URL, "https://api.payments.jpmorgan.com/tsapi/v2/validations/accounts");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(payload));

  result = curl_easy_perform(curl);

  free(payload);
  return result;
}
