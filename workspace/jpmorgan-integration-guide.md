# JPMorgan Chase Validation Services - Production Integration Guide

## Overview
This guide provides step-by-step instructions for integrating with the JPMorgan Chase Validation Services API using organization ID `D3R56WRGSR3R`.

## Quick Start

### 1. Environment Setup
```bash
# Set your environment variables
export JPMORGAN_CLIENT_ID="your-client-id"
export JPMORGAN_PROGRAM_ID="your-program-id"
export JPMORGAN_ORGANIZATION_ID="D3R56WRGSR3R"
```

### 2. API Endpoints

#### Account Validation
**Endpoint:** `POST https://api.payments.jpmorgan.com/tsapi/v2/validations/accounts`
**Purpose:** Validate bank account information and ownership

#### Entity Validation
**Endpoint:** `POST https://api.payments.jpmorgan.com/tsapi/v2/validations/entities`
**Purpose:** Validate individual or business entity information

#### Payroll Validation
**Endpoint:** `POST https://api.payments.jpmorgan.com/tsapi/v2/validations/accounts`
**Purpose:** Validate accounts for payroll processing with transaction context

### 3. Authentication Headers
All requests require:
- `x-client-id`: Your JPMorgan client ID
- `x-program-id`: Your program identifier
- `x-program-id-type`: Program type (e.g., "AVS")
- `x-organization-id`: D3R56WRGSR3R

### 4. Authentication Headers
All requests require:
- `x-client-id`: Your JPMorgan client ID
- `x-program-id`: Your program identifier
- `x-organization-id`: D3R56WRGSR3R

### 5. Using the Integration

#### Command Line
```bash
# Account Validation
curl --request POST \
  --url https://api.payments.jpmorgan.com/tsapi/v2/validations/accounts
