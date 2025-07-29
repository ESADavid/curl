# Perfect Validation System Dockerfile
# Copyright (C) 2024, Perfect Validation Systems

FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV VALIDATION_SYSTEM_CONFIG=/etc/validation-system/validation-system.conf

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libcurl4-openssl-dev \
    libssl-dev \
    libjson-c-dev \
    libhiredis-dev \
    libpq-dev \
    redis-tools \
    postgresql-client \
    curl \
    wget \
    vim \
    htop \
    supervisor \
    && rm -rf /var/lib/apt/lists/*

# Create application user
RUN groupadd -r validation-system && useradd -r -g validation-system validation-system

# Create directories
RUN mkdir -p /opt/validation-system /etc/validation-system /var/log/validation-system /var/lib/validation-system

# Copy source code
COPY . /opt/validation-system/src/
WORKDIR /opt/validation-system/src

# Build the application
RUN mkdir build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/validation-system \
        -DENABLE_VALIDATION=ON \
        -DENABLE_ENHANCED_VALIDATION=ON \
        -DENABLE_PERFECT_VALIDATION=ON && \
    make -j$(nproc) && \
    make install

# Copy configuration files
COPY config/validation-system.conf /etc/validation-system/
COPY config/supervisord.conf /etc/supervisor/conf.d/

# Set permissions
RUN chown -R validation-system:validation-system /opt/validation-system /etc/validation-system /var/log/validation-system /var/lib/validation-system

# Create symlinks for binaries
RUN ln -sf /opt/validation-system/bin/validation-daemon /usr/local/bin/validation-daemon
RUN ln -sf /opt/validation-system/bin/validation-cli /usr/local/bin/validation-cli

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Expose ports
EXPOSE 8080 8443

# Start supervisor
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
