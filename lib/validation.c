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
#include "curl_memory.h"
#include "curl_base64.h"
#include "curl_hmac.h"
#include "curl_md5.h"
#include "curl_sha256.h"
#include "curl_ctype.h"
#include "curl_get_line.h"
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

struct validation_context {
  char *client_id;
  char *program_id;
  char *program_id_type;
  char *endpoint_url;
  char *payload;
  size_t payload_len;
  struct curl_slist *headers;
  struct dynbuf response;
  CURLcode result;
};

static void validation_context_init(struct validation_context *ctx)
{
  memset(ctx, 0, sizeof(*ctx));
  Curl_dyn_init(&ctx->response, DYN_DOH_RESPONSE);
}

static void validation_context_cleanup(struct validation_context *ctx)
{
  free(ctx->client_id);
  free(ctx->program_id);
  free(ctx->program_id_type);
  free(ctx->endpoint_url);
  free(ctx->payload);
  curl_slist_free_all(ctx->headers);
  Curl_dyn_free(&ctx->response);
}

CURLcode curl_easy_setopt_validation(CURL *curl, CURLoption option, ...)
{
  va_list arg;
  CURLcode result = CURLE_OK;
  struct validation_context *ctx = NULL;

  va_start(arg, option);

  /* Get or create validation context */
  ctx = (struct validation_context *)curl->req.validation_ctx;
  if(!ctx) {
    ctx = calloc(1, sizeof(struct validation_context));
    if(!ctx) {
      va_end(arg);
      return CURLE_OUT_OF_MEMORY;
    }
    validation_context_init(ctx);
    curl->req.validation_ctx = ctx;
  }

  switch(option) {
  case CURLOPT_VALIDATION_CLIENT_ID:
    free(ctx->client_id);
    ctx->client_id = strdup(va_arg(arg, char *));
    if(!ctx->client_id)
      result = CURLE_OUT_OF_MEMORY;
    break;

  case CURLOPT_VALIDATION_PROGRAM_ID:
    free(ctx->program_id);
    ctx->program_id = strdup(va_arg(arg, char *));
    if(!ctx->program_id)
      result = CURLE_OUT_OF_MEMORY;
    break;

  case CURLOPT_VALIDATION_PROGRAM_ID_TYPE:
    free(ctx->program_id_type);
    ctx->program_id_type = strdup(va_arg(arg, char *));
    if(!ctx->program_id_type)
      result = CURLE_OUT_OF_MEMORY;
    break;

  case CURLOPT_VALIDATION_ENDPOINT:
    free(ctx->endpoint_url);
    ctx->endpoint_url = strdup(va_arg(arg, char *));
    if(!ctx->endpoint_url)
      result = CURLE_OUT_OF_MEMORY;
    break;

  case CURLOPT_VALIDATION_PAYLOAD:
    free(ctx->payload);
    ctx->payload = strdup(va_arg(arg, char *));
    if(!ctx->payload)
      result = CURLE_OUT_OF_MEMORY;
    else
      ctx->payload_len = strlen(ctx->payload);
    break;

  default:
    result = CURLE_UNKNOWN_OPTION;
    break;
  }

  va_end(arg);
  return result;
}

static CURLcode setup_validation_headers(struct Curl_easy *data,
                                        struct validation_context *ctx)
{
  char header[256];
  struct curl_slist *headers = NULL;

  if(!ctx->client_id || !ctx->program_id) {
    return CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Add required headers */
  msnprintf(header, sizeof(header), "x-client-id: %s", ctx->client_id);
  headers = curl_slist_append(headers, header);
  if(!headers)
    return CURLE_OUT_OF_MEMORY;

  msnprintf(header, sizeof(header), "x-program-id: %s", ctx->program_id);
  headers = curl_slist_append(headers, header);
  if(!headers) {
    curl_slist_free_all(headers);
    return CURLE_OUT_OF_MEMORY;
  }

  if(ctx->program_id_type) {
    msnprintf(header, sizeof(header), "x-program-id-type: %s", ctx->program_id_type);
    headers = curl_slist_append(headers, header);
    if(!headers) {
      curl_slist_free_all(headers);
      return CURLE_OUT_OF_MEMORY;
    }
  }

  headers = curl_slist_append(headers, "Content-Type: application/json");
  if(!headers) {
    curl_slist_free_all(headers);
    return CURLE_OUT_OF_MEMORY;
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  if(!headers) {
    curl_slist_free_all(headers);
    return CURLE_OUT_OF_MEMORY;
  }

  ctx->headers = headers;
  return CURLE_OK;
}

static size_t validation_write_callback(void *contents, size_t size,
                                       size_t nmemb, void *userp)
{
  struct validation_context *ctx = (struct validation_context *)userp;
  size_t realsize = size * nmemb;
  CURLcode result;

  result = Curl_dyn_addn(&ctx->response, contents, realsize);
  if(result)
    return 0;

  return realsize;
}

CURLcode curl_easy_perform_validation(CURL *curl)
{
  struct validation_context *ctx;
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = curl;

  if(!data)
    return CURLE_BAD_FUNCTION_ARGUMENT;

  ctx = (struct validation_context *)data->req.validation_ctx;
  if(!ctx || !ctx->endpoint_url || !ctx->payload) {
    return CURLE_BAD_FUNCTION_ARGUMENT;
  }

  /* Setup headers */
  result = setup_validation_headers(data, ctx);
  if(result)
    return result;

  /* Configure the easy handle for validation */
  curl_easy_setopt(data, CURLOPT_URL, ctx->endpoint_url);
  curl_easy_setopt(data, CURLOPT_HTTPHEADER, ctx->headers);
  curl_easy_setopt(data, CURLOPT_POSTFIELDS, ctx->payload);
  curl_easy_setopt(data, CURLOPT_POSTFIELDSIZE, (long)ctx->payload_len);
  curl_easy_setopt(data, CURLOPT_WRITEFUNCTION, validation_write_callback);
  curl_easy_setopt(data, CURLOPT_WRITEDATA, ctx);

  /* Perform the request */
  result = curl_easy_perform(data);

  return result;
}

char *curl_easy_getinfo_validation_response(CURL *curl)
{
  struct validation_context *ctx;
  struct Curl_easy *data = curl;

  if(!data)
    return NULL;

  ctx = (struct validation_context *)data->req.validation_ctx;
  if(!ctx)
    return NULL;

  return Curl_dyn_ptr(&ctx->response);
}

void curl_easy_cleanup_validation(CURL *curl)
{
  struct validation_context *ctx;
  struct Curl_easy *data = curl;

  if(!data)
    return;

  ctx = (struct validation_context *)data->req.validation_ctx;
  if(ctx) {
    validation_context_cleanup(ctx);
    free(ctx);
    data->req.validation_ctx = NULL;
  }
}
