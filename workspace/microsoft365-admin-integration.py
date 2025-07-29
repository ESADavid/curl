#!/usr/bin/env python3
"""
Microsoft 365 Admin Portal Integration for JPMorgan Validation Services
Handles billing and admin functions for David Leeper Jr (DavidLeeperJr@owlbangroup.com)
"""

import json
import requests
import logging
import os
from datetime import datetime
from typing import Dict, List, Optional
import argparse
import sys
from msal import ConfidentialClientApplication

class Microsoft365AdminIntegration:
    """Microsoft 365 Admin Portal Integration for JPMorgan services"""
    
    def __init__(self):
        self.tenant_id = os.getenv('M365_TENANT_ID', 'owlbangroup.com')
        self.client_id = os.getenv('M365_CLIENT_ID')
        self.client_secret = os.getenv('M365_CLIENT_SECRET')
        self.admin_user = "DavidLeeperJr@owlbangroup.com"
        
        self.setup_logging()
        self.setup_msal_client()
        
    def setup_logging(self):
        """Setup logging configuration"""
        log_dir = "logs"
        os.makedirs(log_dir, exist_ok=True)
        
        log_file = os.path.join(log_dir, f"m365-admin-{datetime.now().strftime('%Y%m%d')}.log")
        
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler(log_file),
                logging.StreamHandler(sys.stdout)
            ]
        )
        self.logger = logging.getLogger(__name__)
        
    def setup_msal_client(self):
        """Setup Microsoft Authentication Library client"""
        if not all([self.client_id, self.client_secret]):
            self.logger.error("Missing Microsoft 365 credentials")
            self.msal_client = None
            return
            
        self.msal_client = ConfidentialClientApplication(
            client_id=self.client_id,
            client_credential=self.client_secret,
            authority=f"https://login.microsoftonline.com/{self.tenant_id}"
        )
        
    def get_access_token(self) -> str:
        """Get Microsoft Graph API access token"""
        if not self.msal_client:
            raise ValueError("MSAL client not configured")
            
        scopes = ["https://graph.microsoft.com/.default"]
        
        try:
            result = self.msal_client.acquire_token_for_client(scopes=scopes)
            if "access_token" in result:
                self.logger.info("Successfully authenticated with Microsoft Graph")
                return result["access_token"]
            else:
                self.logger.error(f"Authentication failed: {result.get('error_description')}")
                raise Exception("Failed to acquire access token")
                
        except Exception as e:
            self.logger.error(f"Token acquisition failed: {e}")
            raise
    
    def get_billing_info(self) -> Dict:
        """Get Microsoft 365 billing information"""
        access_token = self.get_access_token()
        
        headers = {
            'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
        }
        
        # Get subscription information
        url = "https://graph.microsoft.com/v1.0/subscribedSkus"
        
        try:
            response = requests.get(url, headers=headers)
            response.raise_for_status()
            
            billing_data = response.json()
            self.logger.info("Successfully retrieved billing information")
            return billing_data
            
        except requests.exceptions.RequestException as e:
            self.logger.error(f"Failed to get billing info: {e}")
            raise
    
    def get_user_licenses(self, user_email: str) -> Dict:
        """Get license assignments for specific user"""
        access_token = self.get_access_token()
        
        headers = {
            'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
        }
        
        # Get user license details
        url = f"https://graph.microsoft.com/v1.0/users/{user_email}/licenseDetails"
        
        try:
            response = requests.get(url, headers=headers)
            response.raise_for_status()
            
            license_data = response.json()
            self.logger.info(f"Successfully retrieved licenses for {user_email}")
            return license_data
            
        except requests.exceptions.RequestException as e:
            self.logger.error(f"Failed to get user licenses: {e}")
            raise
    
    def assign_license(self, user_email: str, sku_id: str) -> bool:
        """Assign license to user"""
        access_token = self.get_access_token()
        
        headers = {
            'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
        }
        
        url = f"https://graph.microsoft.com/v1.0/users/{user_email}/assignLicense"
        
        payload = {
            "addLicenses": [{"skuId": sku_id}],
            "removeLicenses": []
        }
        
        try:
            response = requests.post(url, json=payload, headers=headers)
            response.raise_for_status()
            
            self.logger.info(f"Successfully assigned license to {user_email}")
            return True
            
        except requests.exceptions.RequestException as e:
            self.logger.error(f"Failed to assign license: {e}")
            return False
    
    def get_payment_methods(self) -> Dict:
        """Get payment methods for the tenant"""
        access_token = self.get_access_token()
        
        headers = {
            'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
        }
        
        # Note: This requires specific billing permissions
        url = "https://graph.microsoft.com/v1.0/billing/paymentMethods"
        
        try:
            response = requests.get(url, headers=headers)
            response.raise_for_status()
            
            payment_data = response.json()
            self.logger.info("Successfully retrieved payment methods")
            return payment_data
            
        except requests.exceptions.RequestException as e:
            self.logger.error(f"Failed to get payment methods: {e}")
            return {}
    
    def create_billing_report(self) -> str:
        """Create comprehensive billing report"""
        try:
            billing_info = self.get_billing_info()
            user_licenses = self.get_user_licenses(self.admin_user)
            
            report = f"""
Microsoft 365 Billing Report
===========================
Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
Admin User: {self.admin_user}
Tenant: {self.tenant_id}

Subscriptions:
"""
            
            for sku in billing_info.get('value', []):
                report += f"""
- SKU: {sku.get('skuPartNumber', 'N/A')}
- Available: {sku.get('consumedUnits', 0)}/{sku.get('prepaidUnits', {}).get('enabled', 0)}
- Status: {sku.get('capabilityStatus', 'N/A')}
"""
            
            report += f"\nUser Licenses for {self.admin_user}:\n"
            for license in user_licenses.get('value', []):
                report += f"- {license.get('skuPartNumber', 'N/A')}: {license.get('servicePlans', [{}])[0].get('servicePlanName', 'N/A')}\n"
            
            return report
            
        except Exception as e:
            self.logger.error(f"Failed to create billing report: {e}")
            return f"Error generating report: {str(e)}"
    
    def validate_environment(self) -> bool:
        """Validate Microsoft 365 environment setup"""
        required_vars = ['M365_CLIENT_ID', 'M365_CLIENT_SECRET']
        missing_vars = [var for var in required_vars if not os.getenv(var)]
        
        if missing_vars:
            self.logger.error(f"Missing required environment variables: {missing_vars}")
            return False
        
        self.logger.info("Microsoft 365 environment validation passed")
        return True

def main():
    """Main entry point for Microsoft 365 admin integration"""
    parser = argparse.ArgumentParser(description='Microsoft 365 Admin Portal Integration')
    parser.add_argument('--billing-info', action='store_true', help='Get billing information')
    parser.add_argument('--user-licenses', help='Get licenses for specific user')
    parser.add_argument('--assign-license', nargs=2, metavar=('USER_EMAIL', 'SKU_ID'), help='Assign license to user')
    parser.add_argument('--payment-methods', action='store_true', help='Get payment methods')
    parser.add_argument('--billing-report', action='store_true', help='Generate comprehensive billing report')
    
    args = parser.parse_args()
    
    # Initialize integration
    m365_integration = Microsoft365AdminIntegration()
    
    # Validate environment
    if not m365_integration.validate_environment():
        sys.exit(1)
    
    # Execute requested action
    if args.billing_info:
        billing_info = m365_integration.get_billing_info()
        print(json.dumps(billing_info, indent=2))
    
    elif args.user_licenses:
        licenses = m365_integration.get_user_licenses(args.user_licenses)
        print(json.dumps(licenses, indent=2))
    
    elif args.assign_license:
        user_email, sku_id = args.assign_license
        success = m365_integration.assign_license(user_email, sku_id)
        print(f"License assignment {'successful' if success else 'failed'}")
    
    elif args.payment_methods:
        payment_methods = m365_integration.get_payment_methods()
        print(json.dumps(payment_methods, indent=2))
    
    elif args.billing_report:
        report = m365_integration.create_billing_report()
        print(report)
        
        # Save report to file
        report_file = f"logs/m365-billing-report-{datetime.now().strftime('%Y%m%d-%H%M%S')}.txt"
        with open(report_file, 'w') as f:
            f.write(report)
        print(f"Report saved to: {report_file}")
    
    else:
        # Default: show billing report
        report = m365_integration.create_billing_report()
        print(report)

if __name__ == "__main__":
    main()
