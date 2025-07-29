#!/bin/bash
# Perfect Validation System Deployment Script
# Copyright (C) 2024, Perfect Validation Systems

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="perfect-validation-system"
BUILD_DIR="build"
INSTALL_DIR="/opt/validation-system"
CONFIG_DIR="/etc/validation-system"
LOG_DIR="/var/log/validation-system"
SERVICE_USER="validation-system"
SERVICE_GROUP="validation-system"

# Functions
log() {
    echo -e "${GREEN}[$(date +'%Y-%m-%d %H:%M:%S')] $1${NC}"
}

error() {
    echo -e "${RED}[$(date +'%Y-%m-%d %H:%M:%S')] ERROR: $1${NC}" >&2
}

warning() {
    echo -e "${YELLOW}[$(date +'%Y-%m-%d %H:%M:%S')] WARNING: $1${NC}"
}

info() {
    echo -e "${BLUE}[$(date +'%Y-%m-%d %H:%M:%S')] INFO: $1${NC}"
}

# Check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        error "This script must be run as root"
        exit 1
    fi
}

# Check dependencies
check_dependencies() {
    log "Checking dependencies..."
    
    # Check for required tools
    local deps=("cmake" "make" "gcc" "pkg-config" "curl")
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            error "Missing dependency: $dep"
            exit 1
        fi
    done
    
    # Check for OpenSSL
    if ! pkg-config --exists openssl; then
        error "OpenSSL development libraries not found"
        exit 1
    fi
    
    log "All dependencies satisfied"
}

# Create system user
create_user() {
    log "Creating system user..."
    
    if ! id "$SERVICE_USER" &>/dev/null; then
        useradd -r -s /bin/false -d "$INSTALL_DIR" "$SERVICE_USER"
        log "Created user: $SERVICE_USER"
    else
        info "User $SERVICE_USER already exists"
    fi
    
    if ! getent group "$SERVICE_GROUP" &>/dev/null; then
        groupadd "$SERVICE_GROUP"
        log "Created group: $SERVICE_GROUP"
    fi
    
    usermod -a -G "$SERVICE_GROUP" "$SERVICE_USER"
}

# Create directories
create_directories() {
    log "Creating directories..."
    
    mkdir -p "$INSTALL_DIR"
    mkdir -p "$CONFIG_DIR"
    mkdir -p "$LOG_DIR"
    mkdir -p "/var/lib/validation-system"
    
    chown -R "$SERVICE_USER:$SERVICE_GROUP" "$INSTALL_DIR"
    chown -R "$SERVICE_USER:$SERVICE_GROUP" "$CONFIG_DIR"
    chown -R "$SERVICE_USER:$SERVICE_GROUP" "$LOG_DIR"
    chown -R "$SERVICE_USER:$SERVICE_GROUP" "/var/lib/validation-system"
    
    chmod 755 "$INSTALL_DIR"
    chmod 750 "$CONFIG_DIR"
    chmod 755 "$LOG_DIR"
}

# Build the project
build_project() {
    log "Building project..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DENABLE_VALIDATION=ON \
        -DENABLE_ENHANCED_VALIDATION=ON \
        -DENABLE_PERFECT_VALIDATION=ON
    
    make -j$(nproc)
    
    log "Build completed successfully"
}

# Install the project
install_project() {
    log "Installing project..."
    
    cd "$BUILD_DIR"
    make install
    
    # Install configuration files
    cp ../config/validation-system.conf "$CONFIG_DIR/"
    cp ../config/validation-system.service /etc/systemd/system/
    
    # Set permissions
    chown "$SERVICE_USER:$SERVICE_GROUP" "$CONFIG_DIR/validation-system.conf"
    chmod 640 "$CONFIG_DIR/validation-system.conf"
    
    log "Installation completed"
}

# Configure systemd service
configure_service() {
    log "Configuring systemd service..."
    
    systemctl daemon-reload
    systemctl enable validation-system
    
    log "Systemd service configured"
}

# Setup firewall
setup_firewall() {
    log "Setting up firewall..."
    
    if command -v ufw &> /dev/null; then
        ufw allow 443/tcp
        ufw allow 80/tcp
        log "Firewall rules added"
    elif command -v firewall-cmd &> /dev/null; then
        firewall-cmd --permanent --add-port=443/tcp
        firewall-cmd --permanent --add-port=80/tcp
        firewall-cmd --reload
        log "Firewall rules added"
    else
        warning "No firewall management tool found"
    fi
}

# Setup monitoring
setup_monitoring() {
    log "Setting up monitoring..."
    
    # Create logrotate configuration
    cat > /etc/logrotate.d/validation-system << EOF
$LOG_DIR/*.log {
    daily
    rotate 30
    compress
    delaycompress
    missingok
    notifempty
    create 644 $SERVICE_USER $SERVICE_GROUP
    postrotate
        systemctl reload validation-system
    endscript
}
EOF
    
    # Create monitoring script
    cat > /usr/local/bin/validation-monitor << 'EOF'
#!/bin/bash
# Validation System Health Monitor

SERVICE="validation-system"
LOG_FILE="/var/log/validation-system/monitor.log"

check_service() {
    if systemctl is-active --quiet "$SERVICE"; then
        echo "$(date): $SERVICE is running" >> "$LOG_FILE"
        return 0
    else
        echo "$(date): $SERVICE is not running" >> "$LOG_FILE"
        systemctl start "$SERVICE"
        return 1
    fi
}

check_disk_space() {
    local usage=$(df /var/lib/validation-system | tail -1 | awk '{print $5}' | sed 's/%//')
    if [ "$usage" -gt 80 ]; then
        echo "$(date): Disk usage is ${usage}%" >> "$LOG_FILE"
        return 1
    fi
    return 0
}

check_memory() {
    local usage=$(free | grep Mem | awk '{printf "%.0f", $3/$2 * 100.0}')
    if [ "$usage" -gt 80 ]; then
        echo "$(date): Memory usage is ${usage}%" >> "$LOG_FILE"
        return 1
    fi
    return 0
}

# Run checks
check_service
check_disk_space
check_memory
EOF
    
    chmod +x /usr/local/bin/validation-monitor
    
    # Add cron job for monitoring
    (crontab -l 2>/dev/null; echo "*/5 * * * * /usr/local/bin/validation-monitor") | crontab -
    
    log "Monitoring setup completed"
}

# Start service
start_service() {
    log "Starting service..."
    
    systemctl start validation-system
    systemctl status validation-system
    
    log "Service started successfully"
}

# Main deployment function
main() {
    log "Starting deployment of Perfect Validation System..."
    
    check_root
    check_dependencies
    create_user
    create_directories
    build_project
    install_project
    configure_service
    setup_firewall
    setup_monitoring
    start_service
    
    log "Deployment completed successfully!"
    log "Validation system is now running and ready for use"
    log "Configuration file: $CONFIG_DIR/validation-system.conf"
    log "Logs: $LOG_DIR/"
    log "Service status: systemctl status validation-system"
}

# Execute main function
main "$@"
