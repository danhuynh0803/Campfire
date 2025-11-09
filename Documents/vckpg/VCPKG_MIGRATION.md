# 🚀 vcpkg 遷移指南

## 概述

本指南說明如何將 Campfire 的依賴管理從 **git submodules** 遷移到 **vcpkg** 包管理器。

## 📊 當前狀態

### 現有依賴方式（git submodules）

```
Vendor/
├── assimp/          (submodule - 500MB+)
├── bullet3/         (submodule - 200MB+)
├── glfw/            (submodule - 50MB)
├── spdlog/          (submodule - 30MB)
├── RuntimeCompiledCPlusPlus/ (submodule - 100MB)
└── ... 更多 submodules
```

**問題**:
- 倉庫體積大（>1GB）
- clone 速度慢
- 更新依賴困難
- 跨平台配置複雜

### 推薦方式（vcpkg）

```
vcpkg.json          (依賴清單 - 2KB)
Vendor/
├── RuntimeCompiledCPlusPlus/  (唯一保留的 submodule)
├── glad/                       (本地源碼)
└── fmod/                       (專有庫)
```

**優勢**:
- ✅ 倉庫體積小（<100MB）
- ✅ 快速 clone
- ✅ 統一跨平台管理
- ✅ 自動依賴解析
- ✅ 版本控制簡單

## 🎯 遷移方案

### 選項 1：完全遷移到 vcpkg（推薦）

適合：新項目、重新開始的開發者

**優點**:
- 最乾淨的方案
- 最小的倉庫體積
- 最佳的依賴管理

**缺點**:
- 需要所有開發者安裝 vcpkg
- CI/CD 需要更新

### 選項 2：混合方案（當前實現）

適合：現有項目、漸進式遷移

**優點**:
- 可以逐步遷移
- 保持向後兼容
- 靈活性高

**缺點**:
- 需要維護兩套配置

### 選項 3：保持 submodules

適合：不想改變現有工作流程

**優點**:
- 無需改變
- 開發者熟悉

**缺點**:
- 倉庫體積大
- 更新依賴困難

## 📦 可通過 vcpkg 管理的依賴

| 庫名 | vcpkg 包名 | 狀態 | 建議 |
|------|-----------|------|------|
| assimp | `assimp` | ✅ v5.4.3+ | 遷移 |
| bullet3 | `bullet3` | ✅ | 遷移 |
| glfw | `glfw3` | ✅ | 遷移 |
| glm | `glm` | ✅ | 遷移 |
| imgui | `imgui` | ✅ | 遷移 |
| spdlog | `spdlog` | ✅ v1.16.0+ | 遷移 |
| fmt | `fmt` | ✅ v12.0+ | 遷移 |
| nlohmann-json | `nlohmann-json` | ✅ | 遷移 |
| entt | `entt` | ✅ | 遷移 |
| lua | `lua` | ✅ | 遷移 |
| stb | `stb` | ✅ | 遷移 |
| freetype | `freetype` | ✅ | 可選 |
| Vulkan | `vulkan` | ✅ headers | 可選 |

### ❌ 無法通過 vcpkg 管理的依賴

| 庫名 | 原因 | 建議處理方式 |
|------|------|------------|
| **RuntimeCompiledCPlusPlus** | 不在 vcpkg | **保持 submodule** ⭐ |
| glad | 配置生成器 | 保持本地源碼 |
| tracy | 需要特定配置 | 保持本地源碼 |
| fmod | 專有庫 | 保持本地 |
| font-awesome-5 | 字體資源 | 保持本地 |

## 🛠️ 實施步驟

### 步驟 1: 安裝 vcpkg

**Windows**:
```powershell
# 安裝到 C:\vcpkg
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# 設置環境變量（PowerShell 管理員）
[System.Environment]::SetEnvironmentVariable(
  'VCPKG_ROOT', 
  'C:\vcpkg', 
  'Machine'
)
[System.Environment]::SetEnvironmentVariable(
  'CMAKE_TOOLCHAIN_FILE', 
  'C:\vcpkg\scripts\buildsystems\vcpkg.cmake', 
  'Machine'
)
```

**Linux**:
```bash
# 安裝到 ~/vcpkg
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh

# 添加到 ~/.bashrc
echo 'export VCPKG_ROOT=~/vcpkg' >> ~/.bashrc
echo 'export CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake' >> ~/.bashrc
source ~/.bashrc
```

### 步驟 2: 準備 Campfire 項目

```bash
cd Campfire

# 1. 只保留 RuntimeCompiledCPlusPlus submodule
git submodule deinit -f Vendor/assimp
git submodule deinit -f Vendor/bullet3
git submodule deinit -f Vendor/glfw
git submodule deinit -f Vendor/glm
git submodule deinit -f Vendor/spdlog
# ... 其他要移除的 submodules

# 2. 保留 RuntimeCompiledCPlusPlus
git submodule update --init --recursive Vendor/RuntimeCompiledCPlusPlus

# 3. 切換到 vcpkg CMake 配置
cp Vendor/CMakeLists.txt Vendor/CMakeLists.txt.submodules
cp Vendor/CMakeLists.txt.vcpkg Vendor/CMakeLists.txt
```

### 步驟 3: 配置和構建

```powershell
# Windows
rm -r -fo Build
cmake -B Build -G "Visual Studio 17 2022" -A x64

# 第一次會自動安裝所有依賴（需要 20-40 分鐘）
cmake --build Build --config Release
```

```bash
# Linux
rm -rf build
cmake -B build

cmake --build build --config Release
```

### 步驟 4: 驗證

```bash
# 檢查安裝的依賴
ls Build/vcpkg_installed/x64-windows/  # Windows
ls build/vcpkg_installed/x64-linux/    # Linux

# 構建測試
cmake --build Build --config Debug
cmake --build Build --config Release
```

## 📝 配置文件說明

### vcpkg.json

定義項目依賴和版本要求：

```json
{
  "name": "campfire",
  "version": "0.1.0",
  "dependencies": [
    {
      "name": "assimp",
      "version>=": "5.4.3"
    },
    {
      "name": "spdlog",
      "version>=": "1.16.0",
      "features": ["fmt-external"]
    }
  ],
  "builtin-baseline": "最新的 vcpkg commit hash"
}
```

### vcpkg-configuration.json

配置 vcpkg 註冊表：

```json
{
  "default-registry": {
    "kind": "git",
    "repository": "https://github.com/microsoft/vcpkg",
    "baseline": "commit hash"
  }
}
```

## 🔄 更新 CI/CD

### GitHub Actions (Windows)

```yaml
name: Windows build (vcpkg)

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive  # 只拉取 RuntimeCompiledCPlusPlus

      - name: Setup vcpkg
        uses: lukka/run-vcpkg@v11
        with:
          vcpkgGitCommitId: 'c8696863d371ab7f46545de4e661c3f42d5f5b16'

      - name: Setup Vulkan SDK
        run: |
          Invoke-WebRequest -Uri "https://sdk.lunarg.com/sdk/download/1.3.250.1/windows/VulkanSDK-1.3.250.1-Installer.exe" -OutFile VulkanSDK.exe
          Start-Process -FilePath VulkanSDK.exe -ArgumentList '/S' -Wait
          
      - name: Configure CMake
        run: |
          cmake -B build -G "Visual Studio 17 2022" -A x64 `
            -DCMAKE_TOOLCHAIN_FILE=${{ github.workspace }}/vcpkg/scripts/buildsystems/vcpkg.cmake

      - name: Build
        run: cmake --build build --config Release --parallel 4
```

### GitHub Actions (Linux)

```yaml
name: Ubuntu build (vcpkg)

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive

      - name: Install system dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake libvulkan-dev vulkan-tools

      - name: Setup vcpkg
        uses: lukka/run-vcpkg@v11
        with:
          vcpkgGitCommitId: 'c8696863d371ab7f46545de4e661c3f42d5f5b16'

      - name: Configure CMake
        run: |
          cmake -B build \
            -DCMAKE_TOOLCHAIN_FILE=${{ github.workspace }}/vcpkg/scripts/buildsystems/vcpkg.cmake

      - name: Build
        run: cmake --build build --config Release --parallel $(nproc)
```

## 🎓 最佳實踐

### 1. 固定 vcpkg baseline

在 `vcpkg.json` 中使用固定的 baseline commit：

```json
{
  "builtin-baseline": "c8696863d371ab7f46545de4e661c3f42d5f5b16"
}
```

這確保所有開發者使用相同版本的依賴。

### 2. 使用版本約束

```json
{
  "name": "assimp",
  "version>=": "5.4.3"  // 最低版本
}
```

### 3. 啟用二進制緩存

```bash
# 使用 GitHub Packages 作為緩存
vcpkg install --x-binarysource=clear;nuget,GitHub,readwrite
```

### 4. 混合方案 - RuntimeCompiledCPlusPlus

由於 RuntimeCompiledCPlusPlus 不在 vcpkg，保持為 submodule：

```bash
# .gitmodules 只保留：
[submodule "Vendor/RuntimeCompiledCPlusPlus"]
    path = Vendor/RuntimeCompiledCPlusPlus
    url = https://github.com/danhuynh0803/RuntimeCompiledCPlusPlus
```

## 🐛 常見問題

### Q: vcpkg 第一次構建很慢

**A**: 這是正常的，vcpkg 需要從源碼編譯所有依賴。後續構建會使用緩存。

解決方法：
```bash
# 使用預編譯二進制
vcpkg install --x-binarysource=default
```

### Q: 如何更新依賴版本？

**A**: 更新 `vcpkg.json` 並更新 baseline：

```bash
# 1. 更新 vcpkg
cd ~/vcpkg
git pull

# 2. 獲取最新 commit
git rev-parse HEAD

# 3. 更新 vcpkg.json 中的 baseline
# 4. 清理並重新構建
rm -rf build/vcpkg_installed
cmake -B build
```

### Q: RuntimeCompiledCPlusPlus 能加入 vcpkg 嗎？

**A**: 理論上可以，但需要：
1. 創建 `portfile.cmake` 和 `vcpkg.json`
2. 提交 PR 到 vcpkg 倉庫
3. 維護 port 更新

目前建議保持為 submodule，因為它是您 fork 的版本。

### Q: 可以本地覆蓋 vcpkg 包嗎？

**A**: 可以使用 overlays：

```cmake
set(VCPKG_OVERLAY_PORTS "${CMAKE_SOURCE_DIR}/vcpkg-overlays")
```

## 📈 遷移效果對比

| 指標 | git submodules | vcpkg | 改進 |
|------|----------------|-------|------|
| 倉庫大小 | ~1.2 GB | ~100 MB | **91% ↓** |
| clone 時間 | ~5-10 分鐘 | ~30 秒 | **90% ↓** |
| 更新依賴 | 手動更新 submodule | 修改 JSON | **更簡單** |
| 跨平台 | 需要配置 | 自動處理 | **更好** |
| 依賴解析 | 手動 | 自動 | **更好** |

## 📚 參考資料

- [vcpkg 官方文檔](https://vcpkg.io/)
- [vcpkg GitHub](https://github.com/microsoft/vcpkg)
- [CMake vcpkg 集成](https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html)
- [vcpkg Manifest 模式](https://vcpkg.io/en/docs/users/manifests.html)

---

*最後更新：2025年11月9日*
*為 Campfire 遊戲引擎量身定制的 vcpkg 遷移方案*

