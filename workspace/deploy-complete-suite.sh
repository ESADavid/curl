#!/bin/bash
# Complete Multi-Platform Integration Suite Deployment Script
# JPMorgan + Microsoft 365 + NVIDIA Dashboard Deployment
# Organization: D3R56WRGSR3R
# Admin: DavidLeeperJr@owlbangroup.com

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo -e "${GREEN}[$(date +'%Y-%m-%d %H:%M:%S')] $1${NC}"
}

error() {
    echo -e "${RED}[ERROR] $1${NC}" >&2
}

warning() {
    echo -e "${YELLOW}[WARNING] $1${NC}"
}

info() {
    echo -e "${BLUE}[INFO] $1${NC}"
}

# Configuration
DEPLOYMENT_DIR="/opt/jpmorgan-validation-suite"
LOG_DIR="/var/log/jpmorgan-validation"
SERVICE_NAME="jpmorgan-validation-suite"
VENV_DIR="$DEPLOYMENT_DIR/venv"
CONFIG_DIR="$DEPLOYMENT_DIR/config"

# Check if running as root
if [[ $EUID -eq 0 ]]; then
   error "This script should not be run as root for security reasons"
   exit 1
fi

log "Starting deployment of JPMorgan Validation Services Suite..."

# Create directories
log "Creating deployment directories..."
mkdir -p "$DEPLOYMENT_DIR" "$LOG_DIR" "$CONFIG_DIR" "$DEPLOYMENT_DIR/scripts"

# Check Python version
log "Checking Python version..."
python3 --version || { error "Python 3 is required"; exit 1; }

# Create virtual environment
log "Creating Python virtual environment..."
python3 -m venv "$VENV_DIR"
source "$VENV_DIR/bin/activate"

# Install dependencies
log "Installing required packages..."
pip install --upgrade pip
pip install requests msal gputil psutil torch torchvision torchaudio --extra-index-url https://download.pytorch.org/whl/cu118

# Copy configuration files
log "Setting up configuration files..."
cp workspace/jpmorgan-config.json "$CONFIG_DIR/"
cp workspace/developer-console-updated.html "$DEPLOYMENT_DIR/"
cp workspace/jpmorgan-integration-guide.md "$DEPLOYMENT_DIR/"

# Copy scripts
log "Copying integration scripts..."
cp workspace/jpmorgan-sync-enhanced.py "$DEPLOYMENT_DIR/"
cp workspace/microsoft365-admin-integration.py "$DEPLOYMENT_DIR/"
cp workspace/nvidia-dashboard-integration.py "$DEPLOYMENT_DIR/"

# Create systemd service file
log "Creating systemd service..."
sudo tee "/etc/systemd/system/$SERVICE_NAME.service" > /dev/null <<EOF
[Unit]
Description=JPMorgan Validation Services Suite
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$DEPLOYMENT_DIR
Environment=PATH=$VENV_DIR/bin
Environment=PYTHONPATH=$DEPLOYMENT_DIR
ExecStart=$VENV_DIR/bin/python3 $DEPLOYMENT_DIR/jpmorgan-sync-enhanced.py --sync-type all --data-source local
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

# Create environment file template
log "Creating environment file template..."
cat > "$CONFIG_DIR/.env.template" <<EOF
# JPMorgan Configuration
JPMORGAN_CLIENT_ID=your-jpmorgan-client-id
JPMORGAN_CLIENT_SECRET=your-jpmorgan-client-secret
JPMORGAN_PROGRAM_ID=your-program-id
JPMORGAN_PROGRAM_ID_TYPE=AVS

# Microsoft 365 Configuration
M365_CLIENT_ID=your-microsoft365-client-id
M365_CLIENT_SECRET=your-microsoft365-client-secret

# NVIDIA Configuration
NVIDIA_API_KEY=your-nvidia-api-key

# Logging
LOG_LEVEL=INFO
EOF

# Create startup script
log "Creating startup script..."
cat > "$DEPLOYMENT_DIR/start-services.sh" <<'EOF'
#!/bin/bash
source venv/bin/activate

# Start JPMorgan sync service
python3 jpmorgan-sync-enhanced.py --health-check &
JPMORGAN_PID=$!

# Start Microsoft 365 admin service
python3 microsoft365-admin-integration.py --billing-report &
M365_PID=$!

# Start NVIDIA dashboard
python3 nvidia-dashboard-integration.py --dashboard &
NVIDIA_PID=$!

echo "Services started:"
echo "JPMorgan: $JPMORGAN_PID"
echo "Microsoft 365: $M365_PID"
echo "NVIDIA: $NVIDIA_PID"

# Wait for all services
wait
EOF

chmod +x "$DEPLOYMENT_DIR/start-services.sh"

# Create monitoring script
log "Creating monitoring script..."
cat > "$DEPLOYMENT_DIR/monitor-services.sh" <<'EOF'
#!/bin/bash
source venv/bin/activate

echo "=== JPMorgan Validation Services Status ==="
python3 jpmorgan-sync-enhanced.py --health-check

echo -e "\n=== Microsoft 365 Admin Status ==="
python3 microsoft365-admin-integration.py --billing-report

echo -e "\n=== NVIDIA Dashboard Status ==="
python3 nvidia-dashboard-integration.py --gpu-info

echo -e "\n=== System Resources ==="
python3 nvidia-dashboard-integration.py --system-info
EOF

chmod +x "$DEPLOYMENT_DIR/monitor-services.sh"

# Create Docker configuration
log "Creating Docker configuration..."
cat > "$DEPLOYMENT_DIR/Dockerfile" <<'EOF'
FROM nvidia/cuda:11.8-devel-ubuntu20.04

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-venv \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY requirements.txt .
RUN pip3 install -r requirements.txt

COPY . .

EXPOSE 8080

CMD ["python3", "nvidia-dashboard-integration.py", "--dashboard"]
EOF

cat > "$DEPLOYMENT_DIR/requirements.txt" <<EOF
requests>=2.28.0
msal>=1.20.0
gputil>=1.4.0
psutil>=5.9.0
torch>=1.13.0
torchvision>=0.14.0
EOF

# Create systemd service for monitoring
log "Creating monitoring service..."
sudo tee "/etc/systemd/system/${SERVICE_NAME}-monitor.service" > /dev/null <<EOF
[Unit]
Description=JPMorgan Validation Services Monitoring
After=network.target

[Service]
Type=oneshot
User=$USER
WorkingDirectory=$DEPLOYMENT_DIR
Environment=PATH=$VENV_DIR/bin
ExecStart=$DEPLOYMENT_DIR/monitor-services.sh
EOF

# Create cron job for regular monitoring
log "Setting up cron job for monitoring..."
(crontab -l 2>/dev/null; echo "*/15 * * * * $DEPLOYMENT_DIR/monitor-services.sh >> $LOG_DIR/monitor.log 2>&1") | crontab -

# Create nginx configuration for web dashboard
log "Creating nginx configuration..."
sudo tee "/etc/nginx/sites-available/jpmorgan-validation" > /dev/null <<EOF
server {
    listen 80;
    server_name localhost;

    location / {
        root $DEPLOYMENT_DIR;
        index nvidia-dashboard.html;
    }

    location /api/health {
        proxy_pass http://localhost:8080;
    }

    location /logs {
        alias $LOG_DIR;
        autoindex on;
    }
}
EOF

# Enable nginx site
sudo ln -sf "/etc/nginx/sites-available/jpmorgan-validation" "/etc/nginx/sites-enabled/"

# Create deployment summary
log "Creating deployment summary..."
cat > "$DEPLOYMENT_DIR/DEPLOYMENT_SUMMARY.md" <<EOF
# JPMorgan Validation Services Suite - Deployment Summary

## Overview
Complete multi-platform integration suite deployed for organization D3R56WRGSR3R

## Components Deployed

### 1. JPMorgan Validation Services
- **Location**: $DEPLOYMENT_DIR/jpmorgan-sync-enhanced.py
- **Features**: OAuth2 authentication, validation sync, error handling
- **Configuration**: $CONFIG_DIR/jpmorgan-config.json

### 2. Microsoft 365 Admin Portal
- **Location**: $DEPLOYMENT_DIR/microsoft365-admin-integration.py
- **Features**: Billing management, license assignment, admin functions
- **Admin**: DavidLeeperJr@owlbangroup.com

### 3. NVIDIA Dashboard Portal
- **Location**: $DEPLOYMENT_DIR/nvidia-dashboard-integration.py
- **Features**: GPU monitoring, system resources, interactive dashboard
- **Dashboard**: http://localhost/nvidia-dashboard.html

## Configuration Steps

1. Copy environment variables:
   cp $CONFIG_DIR/.env.template $CONFIG_DIR/.env
   # Edit with your actual credentials

2. Start services:
   sudo systemctl enable $SERVICE_NAME
   sudo systemctl start $SERVICE_NAME

3. Monitor logs:
   tail -f $LOG_DIR/jpmorgan-sync-$(date +%Y%m%d).log

## Access Points

- **JPMorgan Dashboard**: http://localhost/developer-console-updated.html
- **NVIDIA Dashboard**: http://localhost/nvidia-dashboard.html
- **Logs**: http://localhost/logs/

## Support
For issues, check logs in $LOG_DIR or run $DEPLOYMENT_DIR/monitor-services.sh
EOF

# Final setup
log "Setting permissions..."
chmod +x "$DEPLOYMENT_DIR"/*.py
chmod 600 "$CONFIG_DIR/.env.template"

# Create deployment complete marker
touch "$DEPLOYMENT_DIR/.deployment-complete"

log "Deployment complete!"
log "Next steps:"
log "1. Configure environment variables in $CONFIG_DIR/.env"
log "2. Start services: sudo systemctl start $SERVICE_NAME"
log "3. Monitor: $DEPLOYMENT_DIR/monitor-services.sh"
log "4. Access dashboard: http://localhost/nvidia-dashboard.html"

echo ""
echo "🚀 JPMorgan Validation Services Suite Successfully Deployed!"
echo "📊 Organization: D3R56WRGSR3R"
echo "👤 Admin: DavidLeeperJr@owlbangroup.com"
echo "📁 Location: $DEPLOYMENT_DIR"
echo "📋 Summary: $DEPLOYMENT_DIR/DEPLOYMENT_SUMMARY.md"
