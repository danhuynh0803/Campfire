#!/usr/bin/env pwsh

# Ensure PowerShell 7
if ($PSVersionTable.PSVersion.Major -lt 7) {
    Write-Error "This script requires PowerShell 7 or later. Current version: $($PSVersionTable.PSVersion)"
    exit 1
}

# Define the Vulkan SDK path
$VULKAN_PATH = "C:\VulkanSDK\1.3.211.0\Bin"
$GLSLC = Join-Path $VULKAN_PATH "glslc.exe"

# Array of shader files to compile
$shaders = @(
    @{
        input = "vkPostProcess.vert"
        output = "vkPostProcess.vert.spv"
    },
    @{
        input = "vkPostProcess.frag"
        output = "vkPostProcess.frag.spv"
    },
    @{
        input = "raytrace.comp"
        output = "raytrace.comp.spv"
    }
)

# PowerShell 7 specific error handling
$ErrorActionPreference = 'Stop'

# Compile each shader
foreach ($shader in $shaders) {
    Write-Information "Compiling $($shader.input) to $($shader.output)..." -InformationAction Continue
    
    # Check if glslc exists
    if (-not (Test-Path -LiteralPath $GLSLC)) {
        Write-Error "Could not find glslc at $GLSLC"
        exit 1
    }
    
    # Check if input shader exists using PowerShell 7's improved path handling
    if (-not (Test-Path -LiteralPath $shader.input)) {
        Write-Error "Could not find shader file $($shader.input)"
        continue
    }
    
    # Compile the shader using PowerShell 7's improved error handling
    try {
        $result = Start-Process -FilePath $GLSLC -ArgumentList @(
            $shader.input,
            '-o',
            $shader.output
        ) -Wait -NoNewWindow -PassThru
        
        if ($result.ExitCode -eq 0) {
            Write-Host "✔️ Successfully compiled $($shader.input)" -ForegroundColor Green
        } else {
            Write-Error "❌ Failed to compile $($shader.input) with exit code $($result.ExitCode)"
        }
    } catch {
        Write-Error "Error compiling $($shader.input): $_"
    }
}

# PowerShell 7 specific pause behavior
Write-Host "`nPress Enter to continue..."
$null = Read-Host
