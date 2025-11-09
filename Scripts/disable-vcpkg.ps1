# 禁用 vcpkg Visual Studio 集成的腳本

Write-Host "`n=== 禁用 vcpkg Visual Studio 集成 ===" -ForegroundColor Cyan

# 方法 1: 使用 vcpkg integrate remove
Write-Host "`n[1] 嘗試使用 vcpkg integrate remove..." -ForegroundColor Yellow
if (Test-Path "C:\Games\vcpkg\vcpkg.exe") {
    Write-Host "找到 vcpkg.exe，正在移除集成..." -ForegroundColor Green
    & "C:\Games\vcpkg\vcpkg.exe" integrate remove
} else {
    Write-Host "未找到 C:\Games\vcpkg\vcpkg.exe" -ForegroundColor Red
    Write-Host "如果 vcpkg 安裝在其他位置，請手動運行：" -ForegroundColor Yellow
    Write-Host "  <vcpkg-path>\vcpkg.exe integrate remove" -ForegroundColor White
}

# 方法 2: 刪除用戶級 MSBuild props 文件
Write-Host "`n[2] 檢查用戶級 vcpkg props 文件..." -ForegroundColor Yellow
$userPropsPath = "$env:LOCALAPPDATA\vcpkg\vcpkg.user.props"
if (Test-Path $userPropsPath) {
    Write-Host "找到: $userPropsPath" -ForegroundColor Green
    Write-Host "刪除中..." -ForegroundColor Yellow
    Remove-Item $userPropsPath -Force
    Write-Host "✅ 已刪除" -ForegroundColor Green
} else {
    Write-Host "未找到用戶級 props 文件" -ForegroundColor Gray
}

# 方法 3: 檢查全局 MSBuild 集成
Write-Host "`n[3] 檢查全局 MSBuild 集成..." -ForegroundColor Yellow
$globalTargets = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Microsoft\VC\v170\vcpkg.targets"
$globalProps = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Microsoft\VC\v170\vcpkg.props"

if (Test-Path $globalTargets) {
    Write-Host "找到: $globalTargets" -ForegroundColor Yellow
    Write-Host "警告: 這是全局文件，建議使用 'vcpkg integrate remove' 而不是手動刪除" -ForegroundColor Red
}
if (Test-Path $globalProps) {
    Write-Host "找到: $globalProps" -ForegroundColor Yellow
    Write-Host "警告: 這是全局文件，建議使用 'vcpkg integrate remove' 而不是手動刪除" -ForegroundColor Red
}

# 方法 4: 清理 Campfire Build 目錄
Write-Host "`n[4] 清理 Campfire Build 目錄..." -ForegroundColor Yellow
$buildPath = "C:\Games\Campfire\Build"
if (Test-Path $buildPath) {
    Write-Host "刪除 $buildPath..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildPath -ErrorAction Continue
    Write-Host "✅ Build 目錄已刪除" -ForegroundColor Green
} else {
    Write-Host "Build 目錄不存在" -ForegroundColor Gray
}

Write-Host "`n=== 完成 ===" -ForegroundColor Cyan
Write-Host "`n下一步:" -ForegroundColor Yellow
Write-Host "1. 重新配置: cmake -B Build -G `"Visual Studio 17 2022`" -A x64" -ForegroundColor White
Write-Host "2. 檢查輸出中是否還有 'VcpkgTripletSelection'" -ForegroundColor White
Write-Host "3. 如果還有，可能需要重啟 Visual Studio 或 PowerShell" -ForegroundColor White


