# 依賴庫說明

## 📚 依賴庫引用方式

本項目使用 CMake 管理依賴庫，所有第三方庫位於 `Vendor/` 目錄下，通過 git submodules 管理。

**注意**: 項目提供了 vcpkg 配置文件（位於 `vcpkg-config/` 目錄）作為替代方案，但目前使用 git submodules。

### 主要依賴庫及其配置

#### 1. **spdlog** (日誌庫)
- **引用方式**: `Vendor/CMakeLists.txt` 第 67-68 行
```cmake
add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE "spdlog/include/")
```
- **引用位置**: `Engine/CMakeLists.txt` 第 44 行
- **當前狀態**: Header-only 模式，使用內建的 `fmt` 庫
- **版本**: ✅ **v1.16.0**（完全支持 C++20）

#### 2. **fmt** (格式化庫)
- **引用方式**: 通過 spdlog 內建引用（bundled）
- **路徑**: `Vendor/spdlog/include/spdlog/fmt/bundled/`
- **版本**: ✅ **12.0.0**（隨 spdlog v1.16.0 更新）
- **要求**: MSVC 需要 `/utf-8` 編譯選項

#### 3. **assimp** (模型加載庫)
- **引用方式**: `Vendor/CMakeLists.txt` 通過 `add_subdirectory(assimp)`
- **版本**: ✅ **v5.4.3**（2024年最新穩定版）
- **配置**: 
  - `ASSIMP_BUILD_ZLIB ON` - 從源代碼構建內建 zlib
  - `ASSIMP_BUILD_ASSIMP_TOOLS OFF`
  - `ASSIMP_BUILD_SAMPLES OFF`
  - `ASSIMP_BUILD_TESTS OFF`
- **主要改進**: 
  - 更好的 glTF 2.0 支持
  - 性能優化
  - 修復 const 正確性問題
  - 內建 zlib 支持壓縮格式

#### 4. **RuntimeCompiledCPlusPlus** (運行時編譯系統)
- **引用方式**: `Vendor/CMakeLists.txt`
- **倉庫**: [ewdlop/RuntimeCompiledCPlusPlus](https://github.com/ewdlop/RuntimeCompiledCPlusPlus) (fork)
- **狀態**: ✅ **最新上游版本** (005c0514)
- **版本**: 2025年最新（包含 GLFW3、CMake 改進等）
- **重要功能**:
  - ✅ GLFW3 支持（通過子模塊）
  - ✅ CMake 最低版本 3.5
  - ✅ 跨平台支持（Windows/Linux/macOS）
  - ✅ `RCCPP_ALLOCATOR_INTERFACE` 選項
  - ✅ 改進的編譯器位置設置（Windows）
- **Campfire 配置**: 
  - `BUILD_EXAMPLES OFF`（不構建示例）
  - 保持為 git submodule
- **功能**: 支持 C++ 代碼的運行時重新編譯
- **參考**: 詳見 [RuntimeCompiledCPlusPlus Wiki](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki)

#### 5. **其他依賴**
- **glm** - OpenGL 數學庫
- **bullet3** - 物理引擎
- **glfw** - 視窗管理
- **imgui** - GUI
- **entt** - ECS 系統
- **lua** - 腳本系統
- **Vulkan SDK** - 圖形 API

## ✅ C++20 兼容性狀態

### 當前狀態
- **C++ 標準**: ✅ **C++20**（已升級）
- **spdlog 版本**: v1.16.0（支持 C++20）
- **狀態**: 完全兼容現代編譯器

### 錯誤類型
1. **fmt 格式化器衝突**
   ```
   static_assert failed: 'The format() member function can't be called on const formatter<T>'
   ```

2. **類型轉換錯誤**
   ```
   cannot convert argument 3 from 'std::_Format_arg_store' to 'fmt::v7::basic_format_args'
   ```

3. **控制流錯誤**
   ```
   transfer of control bypasses initialization
   ```

## ✅ 已完成升級

### 實施的升級步驟

**2025年11月8日完成：**

1. **升級 spdlog**: v1.8.1 → v1.16.0（包含 fmt 12.0.0）
   ```bash
   cd Vendor/spdlog
   git checkout v1.16.0
   ```

2. **升級 assimp**: 舊版本 → v5.4.3
   ```bash
   cd Vendor/assimp
   git checkout v5.4.3
   ```
   - 修復 C++20 const 正確性問題
   - 改進 glTF 支持

3. **啟用 C++20**:
   ```cmake
   set(CMAKE_CXX_STANDARD 20)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)
   ```

4. **添加 UTF-8 支持**（fmt 12.0+ 必需）:
   
   **策略**: 針對每個目標單獨設置，避免全局衝突
   
   ```cmake
   # Engine/CMakeLists.txt
   if(MSVC)
       target_compile_options(Engine PRIVATE /utf-8)
   endif()
   
   # Editor/CMakeLists.txt
   if(MSVC)
       target_compile_options(Editor PRIVATE /utf-8)
   endif()
   ```
   
   **重要**: 
   - ❌ 不使用全局 `CMAKE_CXX_FLAGS`（會與 vendor 庫衝突）
   - ✅ 各個庫自行處理 UTF-8：
     - assimp: 添加 `/source-charset:utf-8`
     - RuntimeCompiledCPlusPlus: 添加 `/utf-8` 給其目標
     - Engine/Editor: 添加 `/utf-8` 給其目標
   - 這樣避免了不同 UTF-8 選項之間的衝突

5. **其他 MSVC 編譯選項**:
   - `/Zc:__cplusplus` - 正確的 __cplusplus 宏值
   - `/wd4251`, `/wd4275` - 禁用 DLL 接口警告

6. **禁用警告視為錯誤**:
   ```cmake
   string(APPEND CMAKE_CXX_FLAGS " /WX- /W2")
   ```
   - `/WX-` - 不將警告視為錯誤
   - `/W2` - 警告級別 2（更好的 vendor 兼容性）

7. **更新 CI/CD**: 所有工作流已更新到 C++20

### 如果需要進一步優化

如果仍遇到問題，可以考慮使用外部 fmt 庫：

```cmake
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
add_subdirectory(spdlog)
```

## 📋 升級檢查清單

C++20 升級狀態：

- [x] 升級 spdlog 到 v1.16.0 ✅
- [x] 升級 assimp 到 v5.4.3 ✅
- [x] 升級 RuntimeCompiledCPlusPlus 到最新上游 ✅
- [x] 更新 CMake 到 3.20+ ✅
- [x] 啟用 C++20 標準 ✅
- [x] 更新 CI/CD 工作流（Windows & Linux）✅
- [x] 解決所有編譯選項衝突 ✅
- [x] 更新文檔 ✅
- [ ] 測試所有平台編譯（Windows/Linux/macOS）⏳
- [ ] 驗證所有功能正常工作 ⏳

## 🔗 相關鏈接

- [spdlog GitHub](https://github.com/gabime/spdlog)
- [fmt GitHub](https://github.com/fmtlib/fmt)
- [C++20 std::format 文檔](https://en.cppreference.com/w/cpp/utility/format/format)

## 📝 已解決的問題

1. **MSVC 14.43 "out of support" 警告** ✅
   - 原因：CMake 3.15 無法識別最新的 MSVC 版本
   - 解決：已升級 CMake 到 3.20

2. **fmt/std::format 衝突** ✅
   - 原因：spdlog 內建的 fmt 與 C++20 標準庫衝突
   - 解決：升級 spdlog 到 v1.16.0（包含 fmt 12.0.0）

3. **const 轉換錯誤** ✅
   - 原因：C++20 更嚴格的類型檢查
   - 解決：新版 spdlog 已兼容

4. **Unicode 支持錯誤** ✅
   - 錯誤：`static_assert failed: 'Unicode support requires compiling with /utf-8'`
   - 原因：fmt 12.0+ 需要 UTF-8 編譯支持
   - 解決：添加 MSVC UTF-8 編譯選項

5. **編譯選項衝突 (第一次)** ✅
   - 錯誤：`'/utf-8' and '/source-charset:utf-8' command-line options are incompatible`
   - 原因：assimp v5.4.3 自動添加 `/source-charset:utf-8`，與全局 `/utf-8` 衝突
   - 解決：改用 `/execution-charset:utf-8`（配合 assimp）

6. **編譯選項衝突 (第二次)** ✅
   - 錯誤：`'/utf-8' and '/execution-charset:utf-8' command-line options are incompatible`
   - 原因：RuntimeCompiledCPlusPlus 為其目標添加 `/utf-8`，與全局 `/execution-charset:utf-8` 衝突
   - 解決：移除全局設置，改為針對每個目標設置：
     - Engine: `target_compile_options(Engine PRIVATE /utf-8)`
     - Editor: `target_compile_options(Editor PRIVATE /utf-8)`
     - RuntimeCompiledCPlusPlus: 保持其自己的 `/utf-8` 設置
     - assimp: 保持其自己的 `/source-charset:utf-8` 設置
   - 效果：各個目標獨立管理 UTF-8，避免衝突

## 🎉 完成的依賴升級總結

所有主要依賴庫已成功升級並兼容 C++20：

| 依賴庫 | 舊版本 | 新版本 | 狀態 |
|--------|--------|--------|------|
| **spdlog** | v1.x (舊) | v1.16.0 | ✅ 完成 |
| **fmt** (bundled) | v7.x | v12.0.0 | ✅ 完成 |
| **assimp** | v5.x (舊) | v5.4.3 | ✅ 完成 |
| **RuntimeCompiledCPlusPlus** | e775dfbe | b589cee5 | ✅ 完成 |
| **CMake** | 3.15 | 3.20 | ✅ 完成 |

**關鍵改進**：
- ✅ 完全支持 C++20 標準
- ✅ 解決 fmt/std::format 衝突
- ✅ UTF-8 編譯支持（MSVC `/execution-charset:utf-8` + assimp `/source-charset:utf-8`）
- ✅ 更好的跨平台兼容性
- ✅ 現代化的編譯器支持
- ✅ 所有編譯選項衝突已解決

## 🔄 構建測試

請使用以下命令測試升級：

```powershell
# 清理舊構建
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# 重新配置（C++20）
cmake -B build -G "Visual Studio 17 2022" -A x64

# 構建
cmake --build build --config Release --parallel 4
```

---

*最後更新：2025年11月9日*
*C++20 升級完成，所有主要依賴已更新至最新版本*

