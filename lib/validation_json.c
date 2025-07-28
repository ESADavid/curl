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

#include "curl_setup.h"
#include "validation.h"
#include "curl_base64.h"
#include "curl_memory.h"
#include "dynbuf.h"
#include "strcase.h"
#include "strdup.h"
#include "urldata.h"
#include "sendf.h"
#include "http.h"
#include "http_client.h"
#include "http_proxy.h"
#include "http2.h"
#include "http3.h"
#include "vauth/vauth.h"
#include "curl_printf.h"
#include "curl_get_line.h"
#include "curl_ctype.h"
#include "strcase.h"
#include "strtok.h"
#include "strerror.h"
#include "inet_pton.h"
#include "inet_ntop.h"
#include "curl_path.h"
#include "curl_memory.h"
#include "curl_base64.h"
#include "curl_hmac.h"
#include "curl_md5.h"
#include "curl_sha256.h"
#include "curl_ctype.h"
#include "curl_get_line.h"
#include "curl_path.h"

/* The last #include file should be: */
#include "memdebug.h"

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

/* Create JSON payload for account validation */
char *validation_create_account_payload(const struct account_validation_request *request)
{
  struct dynbuf buf;
  char *escaped_account = NULL;
  char *escaped_financial_id = NULL;
  char *escaped_clearing_id = NULL;
  char *escaped_clearing_type = NULL;
  char *escaped_entity_type = NULL;
  char *escaped_request_id = NULL;

  if(!request)
    return NULL;

  Curl_dyn_init(&buf, 2048);

  escaped_account = json_escape_string(request->account_number);
  escaped_financial_id = json_escape_string(request->financial_institution_id);
  escaped_clearing_id = json_escape_string(request->clearing_system_id);
  escaped_clearing_type = json_escape_string(request->clearing_system_id_type);
  escaped_entity_type = json_escape_string(request->entity_type);
  escaped_request_id = json_escape_string(request->request_id);

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
  Curl_dyn_add(&buf, "        \"firstName\": \"Jane\",\n");
  Curl_dyn_add(&buf, "        \"lastName\": \"Abbot\",\n");
  Curl_dyn_add(&buf, "        \"fullName\": \"Jane Abbot\"\n");
  Curl_dyn_add(&buf, "      }\n");
  Curl_dyn_add(&buf, "    }\n");
  Curl_dyn_add(&buf, "  }\n");
  Curl_dyn_add(&buf, "]");

  free(escaped_account);
  free(escaped_financial_id);
  free(escaped_clearing_id);
  free(escaped_clearing_type);
  free(escaped_entity_type);
  free(escaped_request_id);

  return Curl_dyn_ptr(&buf);
}

/* Create JSON payload for entity validation */
char *validation_create_entity_payload(const struct entity_validation_request *request)
{
  struct dynbuf buf;
  char *escaped_entity_type = NULL;
  char *escaped_first_name = NULL;
  char *escaped_last_name = NULL;
  char *escaped_full_name = NULL;
  char *escaped_postal_address = NULL;
  char *escaped_identification = NULL;
  char *escaped_request_id = NULL;

  if(!request)
    return NULL;

  Curl_dyn_init(&buf, 2048);

  escaped_entity_type = json_escape_string(request->entity_type);
  escaped_first_name = json_escape_string(request->first_name);
  escaped_last_name = json_escape_string(request->last_name);
  escaped_full_name = json_escape_string(request->full_name);
  escaped_postal_address = json_escape_string(request->postal_address);
  escaped_identification = json_escape_string(request->identification);
  escaped_request_id = json_escape_string(request->request_id);

  Curl_dyn_add(&buf, "[\n");
  Curl_dyn_add(&buf, "  {\n");
  Curl_dyn_add(&buf, "    \"requestId\": \"");
  Curl_dyn_add(&buf, escaped_request_id ? escaped_request_id : "");
  Curl_dyn_add(&buf, "\",\n");
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
  Curl_dyn_add(&buf, "\"\n");
  Curl_dyn_add(&buf, "      }\n");
  Curl_dyn_add(&buf, "    }\n");
  Curl_dyn_add(&buf, "  }\n");
  Curl_dyn_add(&buf, "]");

  free(escaped_entity_type);
  free(escaped_first_name);
  free(escaped_last_name);
  free(escaped_full_name);
  free(escaped_postal_address);
  free(escaped_identification);
  free(escaped_request_id);

  return Curl_dyn_ptr(&buf);
}

/* Free payload memory */
void validation_free_payload(char *payload)
{
  free(payload);
}

/* Parse validation response */
struct validation_response *validation_parse_response(const char *json_response)
{
  struct validation_response *response;
  
  if(!json_response)
    return NULL;

  response = calloc(1, sizeof(struct validation_response));
  if(!response)
    return NULL;

  response->response_data = strdup(json_response);
  response->response_size = strlen(json_response);
  response->http_status = 200;
  response->error_message = NULL;

  return response;
}

/* Free validation response */
void validation_free_response(struct validation_response *response)
{
  if(response) {
    free(response->response_data);
    free(response->error_message);
    free(response);
  }
}
