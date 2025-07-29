# JPMorgan Chase Validation Services API - Complete Integration Solution

## Overview
This is a comprehensive integration of the JPMorgan Chase Validation Services API into the curl project, providing full support for account validation, entity validation, and payroll processing.

## Components

### 1. Core Validation Module (`lib/validation.c`)
- Account validation endpoint support
- Entity validation endpoint support
- Authentication header management
- JSON payload creation
- Response processing

### 2. Payroll Validation Module (`lib/validation_payroll.c`)
- Payroll-specific validation requests
- Employer information handling
- Payroll amount and currency validation
- Transaction context support

### 3. JSON Processing (`lib/validation_json.c`)
- Complex JSON payload serialization
- Response parsing utilities
- String escaping for JSON safety

### 4. Public APIs
- `include/curl/validation.h` - Core validation API
- `include/curl/validation_payroll.h` - Payroll validation API

### 5. Command-line Interface (`src/tool_validation.c`)
- New curl options for validation services
- Direct command-line usage support

## Usage Examples

### Account Validation
```bash
curl --request POST \
  --url https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts \
  --header 'x-client-id: CLIENTID' \
  --header 'x-program-id: VERIAUTH' \
  --data '[{"requestId":"123e4567-e89b-12d3-a456-426614174000","account":{"accountNumber":"12345","financialInstitutionId":{"clearingSystemId":{"id":"122199983","idType":"ABA"}}},"entity":{"individual":{"firstName":"Jane","lastName":"Abbot","fullName":"Jane Abbot"}}}]'
```

### Entity Validation
```bash
curl --request POST \
  --url https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/entities \
  --header 'x-client-id: CLIENTID' \
  --header 'x-program-id: COMPANYINDIVIDUAL' \
  --data '[{"requestId":"123e4567-e89b-12d3-a456-426614174123","entity":{"individual":{"firstName":"JOHN","lastName":"SMITH","fullName":"MR JOHN KAY SMITH MARTINEZ JR","postalAddress":{"addressLine":["123 MAIN ST","APT 1A"],"townName":"TAMPA","countrySubDvsn":"FL","country":"US","postalCode":"33607"}}}}]'
```

### Payroll Validation
```bash
curl --request POST \
  --url https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts \
  --header 'x-client-id: CLIENTID' \
  --header 'x-program-id: VERIAUTH' \
  --data '[{"requestId":"123e4567-e89b-12d3-a456-426614174124","account":{"accountNumber":"12345","financialInstitutionId":{"clearingSystemId":{"id":"122199983","idType":"ABA"}}},"entity":{"individual":{"firstName":"Jane","lastName":"Abbot","fullName":"Jane Abbot"}},"transactions":[{"context":"PAYROLL","amount":{"amount":2500.00,"currency":"USD"}}]}]'
```

## API Endpoints Supported
- **Account Validation**: `/validations/accounts`
- **Entity Validation**: `/validations/entities`
- **Payroll Validation**: `/validations/accounts` (with payroll context)

## Response Codes Handled
- 1002: Open Valid
- 1101: Pass
- 1209: No Information Found
- 2101: Business Pass
- 4609: Potential Hit
- 5609: Potential Hit
- All error codes and edge cases

## Features
- ✅ Complete JPMorgan API integration
- ✅ Authentication header management
- ✅ JSON payload creation and parsing
- ✅ Error handling and response processing
- ✅ Command-line interface support
- ✅ libcurl API support
- ✅ Payroll validation support
- ✅ Production-ready implementation

## Installation
The integration is ready for immediate use with the curl project. All components are implemented and tested for production deployment.
