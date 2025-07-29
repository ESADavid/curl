# Node.js Installation Guide for SonarLint

This guide will help you install Node.js and configure SonarLint properly.

## Step 1: Install Node.js

### Option A: Using Node.js Installer (Recommended)
1. Visit [nodejs.org](https://nodejs.org/)
2. Download the **LTS** version (recommended for most users)
3. Run the installer and follow the installation wizard
4. Restart VSCode after installation

### Option B: Using Chocolatey (Windows Package Manager)
```bash
# Install Chocolatey first (if not installed)
# Then run:
choco install nodejs-lts
```

### Option C: Using Scoop
```bash
# Install Scoop first (if not installed)
# Then run:
scoop install nodejs-lts
```

## Step 2: Verify Installation

After installation, verify Node.js is working:

```bash
# Check Node.js version
node --version

# Check npm version
npm --version
```

## Step 3: Update VSCode Settings (if needed)

The settings have been pre-configured in `.vscode/settings.json`. If Node.js is installed in a different location, update the path:

1. Open VSCode settings (Ctrl+,)
2. Search for "sonarlint.pathToNodeExecutable"
3. Update the path to match your Node.js installation

### Common Node.js Installation Paths:
- **Windows (64-bit)**: `C:\Program Files\nodejs\node.exe`
- **Windows (32-bit)**: `C:\Program Files (x86)\nodejs\node.exe`
- **Custom installation**: Check where you installed it

## Step 4: Test SonarLint

1. Open any JavaScript/TypeScript file in VSCode
2. Look for SonarLint warnings in the Problems panel
3. Check the Output panel → SonarLint for any connection issues

## Troubleshooting

### Issue: "Node.js not found"
- Ensure Node.js is added to your system PATH
- Restart VSCode after installation
- Check the exact installation path

### Issue: "Permission denied"
- Run VSCode as administrator
- Check file permissions for the Node.js executable

### Issue: "SonarLint not working"
- Check the SonarLint output panel for specific errors
- Ensure the Node.js version is compatible (Node.js 14+ recommended)

## Verification Commands

```bash
# Check if Node.js is in PATH
where node

# Check Node.js version
node --version

# Check if SonarLint can find Node.js
# (Check VSCode Output panel → SonarLint)
```

## Alternative Solutions

If you cannot install Node.js system-wide, consider:
- Using Node Version Manager (nvm-windows)
- Using Docker with Node.js container
- Using WSL2 with Node.js on Linux

## Support

If you encounter issues:
1. Check the SonarLint extension output in VSCode
2. Verify the Node.js path in settings.json
3. Restart VSCode after any changes
