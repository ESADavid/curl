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

#include "test.h"
#include "curl/curl.h"
#include "curl/validation.h"
#include "curl/validation_payroll.h"
#include "memdebug.h"

static int test_account_validation_payload(void)
{
    struct account_validation_request req = {
        .account_number = "12345",
        .financial_institution_id = "122199983",
        .clearing_system_id = "122199983",
        .clearing_system_id_type = "ABA",
        .entity_type = "individual",
        .request_id = "123e4567-e89b-12d3-a456-426614174000"
    };
    
    char *payload = validation_create_account_payload(&req);
    if(!payload) {
        fprintf(stderr, "Failed to create account validation payload\n");
        return 1;
    }
    
    /* Verify JSON structure */
    if(strstr(payload, "\"accountNumber\":\"12345\"") == NULL) {
        fprintf(stderr, "Account number not found in payload\n");
        free(payload);
        return 1;
    }
    
    if(strstr(payload, "\"idType\":\"ABA\"") == NULL) {
        fprintf(stderr, "ABA type not found in payload\n");
        free(payload);
        return 1;
    }
    
    free(payload);
    return 0;
}

static int test_entity_validation_payload(void)
{
    struct entity_validation_request req = {
        .entity_type = "individual",
        .first_name = "JOHN",
        .last_name = "SMITH",
        .full_name = "MR JOHN KAY SMITH MARTINEZ JR",
        .request_id = "123e4567-e89b-12d3-a456-426614174123"
    };
    
    char *payload = validation_create_entity_payload(&req);
    if(!payload) {
        fprintf(stderr, "Failed to create entity validation payload\n");
        return 1;
    }
    
    /* Verify JSON structure */
    if(strstr(payload, "\"firstName\":\"JOHN\"") == NULL) {
        fprintf(stderr, "First name not found in payload\n");
        free(payload);
        return 1;
    }
    
    if(strstr(payload, "\"fullName\":\"MR JOHN KAY SMITH MARTINEZ JR\"") == NULL) {
        fprintf(stderr, "Full name not found in payload\n");
        free(payload);
        return 1;
    }
    
    free(payload);
    return 0;
}

static int test_payroll_validation_payload(void)
{
    struct payroll_validation_request req = {
        .request_id = "123e4567-e89b-12d3-a456-426614174124",
        .account_number = "12345",
        .financial_institution_id = "122199983",
        .clearing_system_id = "122199983",
        .clearing_system_id_type = "ABA",
        .first_name = "Jane",
        .last_name = "Abbot",
        .full_name = "Jane Abbot",
        .payroll_amount = "2500.00",
        .payroll_currency = "USD",
        .employer_name = "Acme Corp",
        .employer_id = "123456789"
    };
    
    char *payload = validation_create_payroll_payload(&req);
    if(!payload) {
        fprintf(stderr, "Failed to create payroll validation payload\n");
        return 1;
    }
    
    /* Verify JSON structure */
    if(strstr(payload, "\"context\":\"PAYROLL\"") == NULL) {
        fprintf(stderr, "Payroll context not found in payload\n");
        free(payload);
        return 1;
    }
    
    if(strstr(payload, "\"amount\":2500.00") == NULL) {
        fprintf(stderr, "Payroll amount not found in payload\n");
        free(payload);
        return 1;
    }
    
    free(payload);
    return 0;
}

static int test_json_escaping(void)
{
    const char *test_str = "Test \"string\" with \\special\\ chars";
    char *escaped = json_escape_string(test_str);
    if(!escaped) {
        fprintf(stderr, "Failed to escape string\n");
        return 1;
    }
    
    if(strstr(escaped, "\\\"") == NULL) {
        fprintf(stderr, "Double quotes not escaped\n");
        free(escaped);
        return 1;
    }
    
    if(strstr(escaped, "\\\\") == NULL) {
        fprintf(stderr, "Backslashes not escaped\n");
        free(escaped);
        return 1;
    }
    
    free(escaped);
    return 0;
}

int test_main(void)
{
    int result = 0;
    
    result |= test_account_validation_payload();
    result |= test_entity_validation_payload();
    result |= test_payroll_validation_payload();
    result |= test_json_escaping();
    
    return result;
}
