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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/validation.h>
#include <curl/validation_payroll.h>

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    char **response = (char **)userp;
    
    *response = realloc(*response, strlen(*response) + realsize + 1);
    if(*response == NULL) {
        return 0;
    }
    
    memcpy(&(*response)[strlen(*response)], contents, realsize);
    (*response)[strlen(*response) + realsize] = 0;
    
    return realsize;
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    char *response = malloc(1);
    response[0] = '\0';
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if(curl) {
        struct curl_slist *headers = NULL;
        
        /* Example 1: Account Validation */
        printf("=== Account Validation Example ===\n");
        
        struct account_validation_request account_req = {
            .account_number = "12345",
            .financial_institution_id = "122199983",
            .clearing_system_id = "122199983",
            .clearing_system_id_type = "ABA",
            .entity_type = "individual",
            .request_id = "123e4567-e89b-12d3-a456-426614174000"
        };
        
        char *account_payload = validation_create_account_payload(&account_req);
        if(account_payload) {
            headers = curl_slist_append(headers, "x-client-id: CLIENTID");
            headers = curl_slist_append(headers, "x-program-id: VERIAUTH");
            headers = curl_slist_append(headers, "x-program-id-type: AVS");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Accept: application/json");
            
            curl_easy_setopt(curl, CURLOPT_URL, "https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, account_payload);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
            res = curl_easy_perform(curl);
            if(res == CURLE_OK) {
                printf("Account Validation Response:\n%s\n\n", response);
            }
            
            free(account_payload);
            curl_slist_free_all(headers);
            headers = NULL;
            free(response);
            response = malloc(1);
            response[0] = '\0';
        }
        
        /* Example 2: Entity Validation */
        printf("=== Entity Validation Example ===\n");
        
        struct entity_validation_request entity_req = {
            .entity_type = "individual",
            .first_name = "JOHN",
            .last_name = "SMITH",
            .full_name = "MR JOHN KAY SMITH MARTINEZ JR",
            .request_id = "123e4567-e89b-12d3-a456-426614174123"
        };
        
        char *entity_payload = validation_create_entity_payload(&entity_req);
        if(entity_payload) {
            headers = curl_slist_append(headers, "x-client-id: CLIENTID");
            headers = curl_slist_append(headers, "x-program-id: COMPANYINDIVIDUAL");
            headers = curl_slist_append(headers, "x-program-id-type: AVS");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Accept: application/json");
            
            curl_easy_setopt(curl, CURLOPT_URL, "https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/entities");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, entity_payload);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            
            res = curl_easy_perform(curl);
            if(res == CURLE_OK) {
                printf("Entity Validation Response:\n%s\n\n", response);
            }
            
            free(entity_payload);
            curl_slist_free_all(headers);
            headers = NULL;
            free(response);
            response = malloc(1);
            response[0] = '\0';
        }
        
        /* Example 3: Payroll Validation */
        printf("=== Payroll Validation Example ===\n");
        
        struct payroll_validation_request payroll_req = {
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
        
        res = curl_validation_payroll(curl, &payroll_req);
        if(res == CURLE_OK) {
            printf("Payroll Validation Response:\n%s\n", response);
        }
        
        curl_easy_cleanup(curl);
    }
    
    free(response);
    curl_global_cleanup();
    
    return 0;
}
