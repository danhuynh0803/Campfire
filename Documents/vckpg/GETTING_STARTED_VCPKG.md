# 🎮 Campfire 快速開始指南 (vcpkg 版本)

使用 vcpkg 包管理器構建 Campfire 遊戲引擎的最簡單方式。

## ⚡ 快速開始

### 1️⃣ 安裝 vcpkg

**Windows (PowerShell 管理員)**:
```powershell
# 克隆 vcpkg
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# 設置環境變量
[System.Environment]::SetEnvironmentVariable('VCPKG_ROOT', 'C:\vcpkg', 'User')
[System.Environment]::SetEnvironmentVariable('CMAKE_TOOLCHAIN_FILE', 'C:\vcpkg\scripts\buildsystems\vcpkg.cmake', 'User')

# 重新打開 PowerShell 使環境變量生效
```

**Linux/macOS**:
```bash
# 克隆 vcpkg
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
cd ~/vcpkg
./bootstrap-vcpkg.sh

# 添加到 shell 配置
echo 'export VCPKG_ROOT=~/vcpkg' >> ~/.bashrc
echo 'export CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake' >> ~/.bashrc
source ~/.bashrc
```

### 2️⃣ 克隆 Campfire

```bash
git clone --recursive https://github.com/danhuynh0803/Campfire
cd Campfire
```

**注意**: `--recursive` 只會拉取 RuntimeCompiledCPlusPlus submodule（約 100MB），其他依賴由 vcpkg 管理。

### 3️⃣ 構建

**Windows**:
```powershell
# 配置（首次會自動安裝所有依賴，需要 20-40 分鐘）
cmake -B Build -G "Visual Studio 17 2022" -A x64

# 構建
cmake --build Build --config Release --parallel 4
```

**Linux**:
```bash
# 安裝系統依賴
sudo apt-get update
sudo apt-get install -y cmake libvulkan-dev vulkan-tools

# 配置（首次會自動安裝所有依賴）
cmake -B build

# 構建
cmake --build build --config Release --parallel $(nproc)
```

### 4️⃣ 運行

```bash
# Windows
.\Build\Editor\Release\Editor.exe

# Linux
./build/Editor/Editor
```

## 📦 vcpkg 管理的依賴

這些依賴會自動下載和編譯：

- ✅ assimp (v5.4.3) - 3D 模型加載
- ✅ bullet3 - 物理引擎
- ✅ glfw3 - 視窗管理
- ✅ glm - 數學庫
- ✅ imgui - GUI
- ✅ spdlog (v1.16.0) - 日誌
- ✅ fmt (v12.0.0) - 格式化
- ✅ nlohmann-json - JSON
- ✅ entt - ECS
- ✅ lua - 腳本
- ✅ stb - 圖像加載

## 🔧 自定義配置

### 使用不同的 vcpkg 安裝位置

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 構建特定配置

```bash
# Debug 構建
cmake --build build --config Debug

# Release 構建
cmake --build build --config Release
```

### 清理並重新構建

```bash
# Windows
Remove-Item -Recurse -Force Build
cmake -B Build -G "Visual Studio 17 2022" -A x64

# Linux
rm -rf build
cmake -B build
```

## 🐛 常見問題

### Q: 首次構建很慢

**A**: 這是正常的！vcpkg 需要從源碼編譯所有依賴。這只會發生一次，後續構建會很快。

### Q: vcpkg 安裝失敗

**A**: 確保你有足夠的磁盤空間（至少 10GB）和良好的網絡連接。

### Q: CMake 找不到依賴

**A**: 確認環境變量設置正確：
```bash
# Windows
$env:CMAKE_TOOLCHAIN_FILE

# Linux
echo $CMAKE_TOOLCHAIN_FILE
```

### Q: 我想回到 git submodules

**A**: 
```bash
# 恢復舊的 Vendor/CMakeLists.txt
cp Vendor/CMakeLists.txt.submodules Vendor/CMakeLists.txt

# 初始化所有 submodules
git submodule update --init --recursive
```

## 📚 進階文檔

- **完整遷移指南**: [VCPKG_MIGRATION.md](VCPKG_MIGRATION.md)
- **依賴詳情**: [DEPENDENCIES.md](DEPENDENCIES.md)
- **vcpkg 設置**: [.vcpkg-setup.md](.vcpkg-setup.md)

## 🎯 對比：submodules vs vcpkg

| 特性 | git submodules | vcpkg |
|------|----------------|-------|
| 倉庫大小 | ~1.2 GB | ~100 MB |
| clone 時間 | 5-10 分鐘 | 30 秒 |
| 首次構建 | 快 | 慢（編譯依賴） |
| 後續構建 | 快 | 快 |
| 更新依賴 | 手動 | 自動 |
| 跨平台 | 需配置 | 自動處理 |

**建議**: 
- 新項目或重新開始 → 使用 vcpkg ⭐
- 現有項目快速構建 → 保持 submodules
- CI/CD → 使用 vcpkg（更可靠）

## 💡 提示

1. **二進制緩存**: 啟用 vcpkg 的二進制緩存可以加速構建
2. **並行構建**: 使用 `--parallel` 加快編譯速度
3. **磁盤空間**: 確保有足夠空間（vcpkg 緩存約 5-10GB）

## 🤝 獲取幫助

- 遇到問題？查看 [VCPKG_MIGRATION.md](VCPKG_MIGRATION.md)
- 報告 bug：https://github.com/danhuynh0803/Campfire/issues
- vcpkg 文檔：https://vcpkg.io/

---

*Happy Coding! 🔥*

