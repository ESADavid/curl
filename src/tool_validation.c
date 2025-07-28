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
#include "curl/curl.h"
#include "memdebug.h"

#define VALIDATION_USAGE \
  "Validation options:\n" \
  "     --validation-type TYPE  Type of validation (account|entity)\n" \
  "     --validation-endpoint URL  JPMorgan validation endpoint\n" \
  "     --validation-client-id ID  Client ID for authentication\n" \
  "     --validation-program-id ID  Program ID for authentication\n" \
  "     --validation-program-id-type TYPE  Program ID type (default: AVS)\n" \
  "     --validation-payload FILE  JSON payload file\n" \
  "     --validation-account NUMBER  Account number for validation\n" \
  "     --validation-financial-id ID  Financial institution ID\n" \
  "     --validation-clearing-id ID  Clearing system ID\n" \
  "     --validation-clearing-type TYPE  Clearing system type (ABA|SWIFT|etc)\n" \
  "     --validation-entity-type TYPE  Entity type (individual|organization)\n" \
  "     --validation-first-name NAME  First name for entity validation\n" \
  "     --validation-last-name NAME  Last name for entity validation\n" \
  "     --validation-full-name NAME  Full name for entity validation\n" \
  "     --validation-request-id ID  Request ID (UUID format)\n"

void tool_validation_help(void)
{
  puts(VALIDATION_USAGE);
}
