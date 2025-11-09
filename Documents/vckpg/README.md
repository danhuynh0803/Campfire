# 📦 vcpkg 配置文件（暫未使用）

本目錄包含 vcpkg 包管理器的配置文件和相關文檔，供未來可能的遷移使用。

## 📂 目錄內容

### 配置文件

- `vcpkg.json` - vcpkg 依賴清單（manifest 模式）
- `vcpkg-configuration.json` - vcpkg 註冊表配置
- `CMakeLists.txt.vcpkg` - 使用 vcpkg 的 CMake 配置文件

### 文檔

- `VCPKG_MIGRATION.md` - 完整的 vcpkg 遷移指南
- `GETTING_STARTED_VCPKG.md` - vcpkg 快速開始指南
- `.vcpkg-setup.md` - vcpkg 設置詳細說明
- `CHANGELOG_VCPKG.md` - vcpkg 集成更新日誌
- `WORK_SUMMARY.md` - 完整工作總結

## 🚫 當前狀態

**這些配置目前未被使用。** Campfire 仍然使用 git submodules 管理依賴。

## 🔮 未來使用

如果您想切換到 vcpkg：

1. **複製配置文件到項目根目錄**:
   ```bash
   cp vcpkg-config/vcpkg.json .
   cp vcpkg-config/vcpkg-configuration.json .
   ```

2. **替換 Vendor CMakeLists.txt**:
   ```bash
   cp Vendor/CMakeLists.txt Vendor/CMakeLists.txt.submodules
   cp vcpkg-config/CMakeLists.txt.vcpkg Vendor/CMakeLists.txt
   ```

3. **安裝 vcpkg**:
   ```bash
   git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
   ~/vcpkg/bootstrap-vcpkg.sh
   ```

4. **使用 vcpkg 構建**:
   ```bash
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build build
   ```

## 📚 為什麼保留這些文件？

1. **備選方案**: vcpkg 提供更現代的依賴管理
2. **未來遷移**: 如果決定切換，配置已經準備好
3. **文檔參考**: 詳細的遷移文檔可供參考
4. **社區選擇**: 讓貢獻者選擇他們偏好的方式

## ⚡ vcpkg vs submodules

| 特性 | submodules (當前) | vcpkg (可選) |
|------|-------------------|-------------|
| 倉庫大小 | 1.2 GB | 100 MB |
| clone 時間 | 5-10 分鐘 | 30 秒 |
| 首次構建 | 快 | 慢（編譯依賴） |
| 後續構建 | 快 | 快 |
| 更新依賴 | 手動 | 修改 JSON |
| 跨平台 | 需配置 | 自動 |

## 📖 詳細文檔

請參閱 `VCPKG_MIGRATION.md` 了解：
- 完整的遷移步驟
- vcpkg 安裝和配置
- CI/CD 集成
- 常見問題解答

## 🔗 相關鏈接

- [vcpkg 官方網站](https://vcpkg.io/)
- [vcpkg GitHub](https://github.com/microsoft/vcpkg)
- [CMake vcpkg 集成文檔](https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html)

---

*這些配置文件已準備好，但目前 Campfire 仍使用 git submodules。*

