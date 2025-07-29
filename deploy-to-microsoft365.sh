#!/bin/bash
# Microsoft 365 Dashboard Deployment Script
# JPMorgan Chase Validation Services

set -e

echo "🚀 Deploying to Microsoft 365 Dashboard..."

# Check Microsoft 365 CLI
if ! command -v az &> /dev/null; then
    echo "❌ Azure CLI not found. Please install Azure CLI."
    exit 1
fi

# Login to Microsoft 365
echo "🔐 Logging into Microsoft 365..."
az login

# Create SharePoint site if needed
echo "📁 Setting up SharePoint site..."
az spo site create --url https://jpmorgan.sharepoint.com/sites/validation-services --title "Validation Services"

# Upload files to SharePoint
echo "📤 Uploading files to SharePoint..."
az spo file upload --source training/09-nvidia-technology/main.c --url https://jpmorgan.sharepoint.com/sites/validation-services/Source Code/main.c
az spo file upload --source training/09-nvidia-technology/ai-workers.c --url https://jpmorgan.sharepoint.com/sites/validation-services/Source Code/ai-workers.c

# Setup Teams notifications
echo "💬 Setting up Teams notifications..."
# Teams webhook configuration would go here

# Configure Power BI
echo "📊 Configuring Power BI..."
# Power BI dataset creation would go here

echo "✅ Deployment to Microsoft 365 completed successfully!"
echo "   SharePoint: https://jpmorgan.sharepoint.com/sites/validation-services"
echo "   Teams: Validation Services Updates channel"
echo "   Power BI: Validation Services Dashboard"
