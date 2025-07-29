# Node.js Installation Script for Windows
# This script downloads and installs Node.js LTS version

Write-Host "Starting Node.js LTS installation..." -ForegroundColor Green

# Create temporary directory
$tempDir = "$env:TEMP\nodejs-install"
New-Item -ItemType Directory -Force -Path $tempDir | Out-Null

# Download Node.js LTS installer
$nodeUrl = "https://nodejs.org/dist/v20.11.1/node-v20.11.1-x64.msi"
$installerPath = "$tempDir\nodejs-installer.msi"

Write-Host "Downloading Node.js LTS installer..." -ForegroundColor Yellow
try {
    Invoke-WebRequest -Uri $nodeUrl -OutFile $installerPath
    Write-Host "Download completed successfully!" -ForegroundColor Green
} catch {
    Write-Host "Failed to download Node.js: $_" -ForegroundColor Red
    exit 1
}

# Install Node.js silently
Write-Host "Installing Node.js..." -ForegroundColor Yellow
try {
    Start-Process msiexec.exe -Wait -ArgumentList "/i `"$installerPath`" /quiet /norestart"
    Write-Host "Node.js installed successfully!" -ForegroundColor Green
} catch {
    Write-Host "Failed to install Node.js: $_" -ForegroundColor Red
    exit 1
}

# Verify installation
Write-Host "Verifying installation..." -ForegroundColor Yellow
$env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")

try {
    $nodeVersion = node --version
    $npmVersion = npm --version
    Write-Host "Node.js version: $nodeVersion" -ForegroundColor Green
    Write-Host "npm version: $npmVersion" -ForegroundColor Green
    
    # Update VSCode settings
    $vscodeSettingsPath = "c:\Users\Dell\OneDrive\curl\.vscode\settings.json"
    $settings = @{
        "sonarlint.pathToNodeExecutable" = "C:\Program Files\nodejs\node.exe"
    }
    
    $settings | ConvertTo-Json | Out-File -FilePath $vscodeSettingsPath -Encoding utf8
    Write-Host "VSCode settings updated successfully!" -ForegroundColor Green
    
} catch {
    Write-Host "Node.js verification failed: $_" -ForegroundColor Red
    exit 1
}

# Clean up
Remove-Item -Recurse -Force $tempDir

Write-Host "Installation completed successfully! Please restart VSCode." -ForegroundColor Green
