#!/bin/bash
# JPMorgan Chase Validation Services Training Setup Script

set -e

echo "🚀 Setting up JPMorgan Chase Validation Services Training Environment"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check dependencies
echo "📋 Checking dependencies..."

# Check for curl
if ! command -v curl &> /dev/null; then
    echo -e "${RED}❌ curl is not installed${NC}"
    exit 1
fi

# Check for gcc
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}❌ gcc is not installed${NC}"
    exit 1
fi

# Check for make
if ! command -v make &> /dev/null; then
    echo -e "${RED}❌ make is not installed${NC}"
    exit 1
fi

echo -e "${GREEN}✅ All dependencies satisfied${NC}"

# Create training directories
echo "📁 Creating training directories..."
mkdir -p training/{02-account-validation,03-entity-validation,04-payroll-validation,05-error-handling,06-performance,07-production,08-ai-enhancement}
mkdir -p training/{docs,scripts,examples,tests}

# Make scripts executable
chmod +x training/scripts/*.sh

# Build the basic training module
echo "🔨 Building basic training module..."
cd training/01-basics
make clean && make
cd ../..

echo -e "${GREEN}✅ Training environment setup complete!${NC}"
echo ""
echo "🎯 Next steps:"
echo "   1. cd training/01-basics"
echo "   2. ./basic-training"
echo "   3. Continue with other modules as needed"
