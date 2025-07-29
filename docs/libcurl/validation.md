# JPMorgan Chase Validation Services API - Developer Portal

## Overview
This documentation provides comprehensive guidance for using the JPMorgan Chase Validation Services API integration in curl/libcurl.

## Quick Start

### 1. Account Validation
```c
#include <curl/curl.h>
#include <curl/validation.h>

int main() {
    CURL *curl = curl_easy_init();
    if(curl) {
        struct account_validation_request req = {
            .account_number = "12345",
            .financial_institution_id = "122199983",
            .clearing_system_id = "122199983",
            .clearing_system_id_type = "ABA",
            .entity_type = "individual",
            .request_id = "123e4567-e89b-12d3-a456-426614174000"
        };
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "x-client-id: CLIENTID");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "x-program-id: VERIAUTH");
        
        char *payload = validation_create_account_payload(&req);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
        
        CURLcode res = curl_easy_perform(curl);
        free(payload);
        curl_easy_cleanup(curl);
    }
    return 0;
}
```

### 2. Entity Validation
```c
#include <curl/curl.h>
#include <curl/validation.h>

int main() {
    CURL *curl = curl_easy_init();
    if(curl) {
        struct entity_validation_request req = {
            .entity_type = "individual",
            .first_name = "JOHN",
            .last_name = "SMITH",
            .full_name = "MR JOHN KAY SMITH MARTINEZ JR",
            .request_id = "123e4567-e89b-12d3-a456-426614174123"
        };
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/entities");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "x-client-id: CLIENTID");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "x-program-id: COMPANYINDIVIDUAL");
        
        char *payload = validation_create_entity_payload(&req);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
        
        CURLcode res = curl_easy_perform(curl);
        free(payload);
        curl_easy_cleanup(curl);
    }
    return 0;
}
```

### 3. Payroll Validation
```c
#include <curl/curl.h>
#include <curl/validation_payroll.h>

int main() {
    CURL *curl = curl_easy_init();
    if(curl) {
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
        
        CURLcode res = curl_validation_payroll(curl, &req);
        curl_easy_cleanup(curl);
    }
    return 0;
}
```

## Command Line Usage

### Account Validation
```bash
curl --request POST \
  --url https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts \
  --header 'x-client-id: CLIENTID' \
  --header 'x-program-id: VERIAUTH' \
  --header 'x-program-id-type: AVS' \
  --data '[{"requestId":"123e4567-e89b-12d3-a456-426614174000","account":{"accountNumber":"12345","financialInstitutionId":{"clearingSystemId":{"id":"122199983","idType":"ABA"}}},"entity":{"individual":{"firstName":"Jane","lastName":"Abbot","fullName":"Jane Abbot"}}}]'
```

### Entity Validation
```bash
curl --request POST \
  --url https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/entities \
  --header 'x-client-id: CLIENTID' \
  --header 'x-program-id: COMPANYINDIVIDUAL' \
  --header 'x-program-id-type: AVS' \
  --data '[{"requestId":"123e4567-e89b-12d3-a456-426614174123","entity":{"individual":{"firstName":"JOHN","lastName":"SMITH","fullName":"MR JOHN KAY SMITH MARTINEZ JR","postalAddress":{"addressLine":["123 MAIN ST","APT 1A"],"townName":"TAMPA","countrySubDvsn":"FL","country":"US","postalCode":"33607"}}}}]'
```

### Payroll Validation
```bash
curl --request POST \
  --url https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts \
  --header 'x-client-id: CLIENTID' \
  --header 'x-program-id: VERIAUTH' \
  --header 'x-program-id-type: AVS' \
  --data '[{"requestId":"123e4567-e89b-12d3-a456-426614174124","account":{"accountNumber":"12345","financialInstitutionId":{"clearingSystemId":{"id":"122199983","idType":"ABA"}}},"entity":{"individual":{"firstName":"Jane","lastName":"Abbot","fullName":"Jane Abbot"}},"transactions":[{"context":"PAYROLL","amount":{"amount":2500.00,"currency":"USD"}}]}]'
```

## API Reference

### Core Functions

#### `validation_create_account_payload()`
Creates JSON payload for account validation requests.

**Parameters:**
- `request`: Account validation request structure

**Returns:** JSON string (must be freed)

#### `validation_create_entity_payload()`
Creates JSON payload for entity validation requests.

**Parameters:**
- `request`: Entity validation request structure

**Returns:** JSON string (must be freed)

#### `validation_create_payroll_payload()`
Creates JSON payload for payroll validation requests.

**Parameters:**
- `request`: Payroll validation request structure

**Returns:** JSON string (must be freed)

#### `curl_validation_payroll()`
Performs payroll validation request.

**Parameters:**
- `curl`: CURL handle
- `request`: Payroll validation request structure

**Returns:** CURLcode

## Response Codes

| Code | Message | Description |
|------|---------|-------------|
| 1002 | Open Valid | Account is open and valid |
| 1101 | Pass | Entity validation passed |
| 1209 | No Information Found | No information available |
| 2101 | Business Pass | Business entity validation passed |
| 4609 | Potential Hit | Individual screening potential match |
| 5609 | Potential Hit | Business screening potential match |

## Error Handling
All API responses include proper error handling with detailed error codes and messages as specified in the JPMorgan Validation Services API documentation.

## Support
For technical support, refer to the JPMorgan Chase Validation Services API documentation or contact JPMorgan Technical Services Support.
