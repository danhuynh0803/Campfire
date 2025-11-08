# Campfire
[![Ubuntu build](https://github.com/danhuynh0803/Campfire/workflows/Ubuntu%20build/badge.svg)](https://github.com/danhuynh0803/Campfire/actions/workflows/c-cpp.yml)
[![Windows build](https://github.com/danhuynh0803/Campfire/workflows/Windows%20build/badge.svg)](https://github.com/danhuynh0803/Campfire/actions/workflows/windows-build.yml)
[![CodeQL Advanced](https://github.com/danhuynh0803/Campfire/actions/workflows/codeql.yml/badge.svg)](https://github.com/danhuynh0803/Campfire/actions/workflows/codeql.yml)

## Summary
Campfire is an in-progress game engine built using OpenGL and Vulkan. Most of the engine's features are currently disabled, as we work through switching to Vulkan, but are available on the [OpenGL branch](https://github.com/danhuynh0803/Campfire/tree/OpenGL).

## Contributing
When creating issues or pull requests, please use the following labels to help organize and prioritize work:

**平台標籤 (Platform):**
- `platform: linux` - Linux 相關問題
- `platform: windows` - Windows 相關問題
- `platform: macos` - macOS 相關問題
- `platform: cross-platform` - 跨平台問題

**類型標籤 (Type):**
- `type: bug` - 錯誤報告
- `type: enhancement` - 功能請求
- `type: documentation` - 文檔改進
- `type: question` - 問題諮詢
- `type: performance` - 性能改進

**組件標籤 (Component):**
- `component: vulkan` - Vulkan 渲染
- `component: opengl` - OpenGL 渲染
- `component: editor` - 編輯器層
- `component: scripting` - Lua 腳本系統
- `component: physics` - 物理系統
- `component: particles` - 粒子系統

**優先級標籤 (Priority):**
- `priority: critical` - 緊急
- `priority: high` - 高優先級
- `priority: medium` - 中優先級
- `priority: low` - 低優先級

## Sample Images
![SampleScene](https://i.imgur.com/jZDVC6l.jpg)

## Realtime RT
![RT-Scene](https://i.imgur.com/kmzx7xv.png)

## Dependencies
Vulkan SDK: [1.2.148.1](https://vulkan.lunarg.com/sdk/home)

## Build Instructions
```bash
git clone --recursive https://github.com/danhuynh0803/Campfire
cd Campfire
cd Build
```

Now generate a project file or makefile for your platform. If you want to use a particular IDE, make sure it is installed; don't forget to set the Start-Up Project in Visual Studio or the Target in Xcode.

```bash
# UNIX Makefile
cmake ..
make -j4

# Microsoft Windows
cmake -G "Visual Studio 16 2019" ..
```
## Library
Functionality           | Library
----------------------- | ------------------------------------------
Mesh Loading            | [assimp](https://github.com/assimp/assimp)
Physics                 | [bullet](https://github.com/bulletphysics/bullet3)
OpenGL Function Loader  | [glad](https://github.com/Dav1dde/glad)
Windowing and Input     | [glfw](https://github.com/glfw/glfw)
OpenGL Mathematics      | [glm](https://github.com/g-truc/glm)
Texture Loading         | [stb](https://github.com/nothings/stb)
Logging                 | [spdlog](https://github.com/gabime/spdlog)
Scene Loading/Saving    | [json](https://github.com/nlohmann/json)
GUI                     | [imgui](https://github.com/ocornut/imgui)
Audio                   | [FMOD](https://www.fmod.com/studio)
Scripting               | [Lua 5.4.0](http://www.lua.org/download.html)
Entity Component System | [entt](https://github.com/skypjack/entt)

## Contributions by ewdlop

### 主要工作領域（經 git 歷史驗證）
- **Particle Effect System**: 實現 Color Over Lifetime、Size Over Lifetime、隨機顏色生成等功能
- **Lua Scripting**: 重構和擴展 LuaEntity（400+ 行），創建 LuaCamera，添加 Lua 全局變量 Inspector
- **File System**: 擴展跨平台文件系統 API，完善 Windows 文件系統文檔
- **Asset Browser**: 添加右鍵選單、雙擊瀏覽、編輯按鈕等功能增強
- **Transform Widget**: 實現完整撤銷/重做系統，與 Lua Transform 集成
- **Vulkan Layer**: 性能優化和 bug 修復，實現 shader 文件監視器
- **Editor UI**: 集成 Font Awesome 圖標，添加鍵盤快捷鍵，改進配色方案

*詳細的提交參考信息請見文檔末尾。*

---

## 經驗證的實際貢獻詳情

以下貢獻已通過 git 提交歷史驗證：

### Particle Effect System
- **Color Over Lifetime**: 添加粒子顏色隨時間變化功能（提交 `e854862`）
- **Size Over Lifetime**: 實現粒子大小隨時間變化（提交 `7d50936`）
- **Random Spawn Color**: 添加隨機生成顏色選項（提交 `291b45f`）
- **基礎加速度選項**（提交 `5a43774`）
- **修復 Inspector bug**（提交 `7004b30`）

### Lua Scripting System
- **重組和擴展 LuaEntity**: 重構 LuaScript，添加 400+ 行代碼（提交 `c3a7e26`）
- **LuaCamera 組件**: 創建新的 Lua 攝像機接口（提交 `2e6b3ac`）
- **Lua 全局變量 Inspector**: 在 Inspector Widget 中添加 Lua 全局變量顯示（提交 `91db8b1`）
- **嵌套 Lua 表支持**: 為嵌套 Lua 表添加 ShowJsonObject 方法（提交 `2e6b3ac`）

### File System
- **Windows FileSystem 文檔**: 為 WindowsFileSystem.cpp 添加詳細註釋（提交 `8941e37`）
- **跨平台文件系統擴展**: 擴展 FileSystem API 以支持 AssetBrowser 功能（提交 `3475b07`）

### Asset Browser 增強
- **右鍵選單**: 為資產瀏覽器添加右鍵上下文選單（提交 `3475b07` - 96 行變更）
- **雙擊瀏覽**: 實現雙擊打開資產功能（提交 `5039c4a`）
- **編輯按鈕**: 為腳本資產添加編輯按鈕（提交 `55a1746`）

### Transform Widget 改進
- **完整的撤銷/重做**: 實現 TransformWidget 的撤銷/重做系統（提交 `40ef4b3`）
- **Lua Transform 集成**: 將 TransformComponent 與 LuaEntity 連接（提交 `56e36e2`）
- **代碼文檔**: 添加詳細註釋（提交 `fc42dbf`）

### Vulkan 渲染層調試
- **性能優化**: 移除不必要的 `graphicsQueue.waitIdle()` 調用（提交 `ec441d1`）
- **圖像佈局修復**: 修復 VulkanSwapChain 中的圖像佈局轉換（提交 `8819c68`, `edbe0c6`）
- **Shader 文件監視器**: 實現 Vulkan shader 的臨時文件監視器（提交 `bb55ea1`）

### Editor Layer 改進
- **Font Awesome 集成**: 添加 Font Awesome 5 圖標字體支持（提交 `23661b5`, `ef54aa7`）
- **快捷鍵**: 為命令管理器添加撤銷/重做鍵盤快捷鍵（提交 `18d9b6f`）
- **UI 改進**: 改進 Hierarchy 和 Inspector widget 的配色方案（提交 `3154280`, `5d27c45`）

### 其他貢獻
- **CMake 升級**: 將 CMAKE_CXX_STANDARD 設置為 C++20（提交 `8bbd3e4`）
- **現代線程**: 將 `std::thread` 切換到 C++20 `jthread`（提交 `ea5eafb`）
- **Windows 信息**: 添加邏輯處理器信息顯示（提交 `52f1ea4`）

**注意**: 早期版本中聲稱「實現」或「創建」整個 Editor Layer、Hierarchy Widget、Inspector Widget 等是不準確的。實際貢獻主要是功能增強、bug 修復和特定功能添加，而非這些系統的初始實現。

---

## AI 生成內容附錄

> **[警告]** 以下內容由 AI 自動生成，並非項目官方文檔的一部分。這些內容可能包含不準確或過時的信息，僅供參考。

### 在 Linux 上進行 C++ 編程

要在 Linux 上進行 C++ 編程，您需要安裝編譯器、設置開發環境，並熟悉基本的編譯和調試過程。以下是詳細步驟：

1. **安裝編譯器：**
    大多數 Linux 發行版默認包含 GNU 編譯器集合 (GCC)。您可以通過以下命令檢查是否已安裝：

```bash
gcc -v
```

如果未安裝，請使用以下命令進行安裝：

   - **Debian/Ubuntu：**

```bash
sudo apt-get update
sudo apt-get install build-essential gdb
```

   - **Red Hat/Fedora：**

```bash
sudo dnf group install "Development Tools"
```

這些命令將安裝 GCC 編譯器和 GDB 調試器。

2. **編寫 C++ 程序：**
    使用您喜歡的文本編輯器（如 Vim、Nano 或 Visual Studio Code）創建一個新的 C++ 源文件。例如，使用 Nano 創建一個名為 `hello.cpp` 的文件：

```bash
nano hello.cpp
```

在文件中輸入以下代碼：

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

保存並關閉編輯器。

3. **編譯程序：**
    在終端中，導航到包含 `hello.cpp` 的目錄，並使用以下命令編譯：

```bash
g++ -o hello hello.cpp
```

此命令使用 `g++` 編譯器將源代碼編譯為名為 `hello` 的可執行文件。

4. **運行程序：**
    編譯成功後，使用以下命令運行程序：

```bash
./hello
```

您應會在終端中看到輸出：`Hello, World!`。

5. **使用集成開發環境 (IDE)：**
    雖然命令行工具功能強大，但使用 IDE 可以提高開發效率。Visual Studio Code 是一個流行的選擇。您可以通過以下步驟在 Linux 上設置 Visual Studio Code 進行 C++ 開發：

   - **安裝 Visual Studio Code：**
      從 [Visual Studio Code 官方網站](https://code.visualstudio.com/) 下載適用於您發行版的安裝包，並按照指示進行安裝。

   - **安裝 C++ 擴展：**
      啟動 Visual Studio Code，點擊左側的擴展圖標（或按 `Ctrl+Shift+X`），搜索 `C++`，然後安裝由 Microsoft 提供的 C/C++ 擴展。

   - **配置編譯和調試環境：**
      按照 [Visual Studio Code 的官方文檔](https://code.visualstudio.com/docs/cpp/config-linux) 中的指南，設置 `tasks.json` 和 `launch.json` 以配置編譯和調試任務。

6. **調試程序：**
    使用 GDB 調試器，您可以在終端中運行以下命令啟動調試：

```bash
gdb ./hello
```

在 GDB 提示符下，輸入 `run` 運行程序，使用 `break` 設置斷點，使用 `step` 或 `next` 單步執行代碼。

通過以上步驟，您可以在 Linux 上成功進行 C++ 開發。隨著經驗的積累，您可以探索更多高級工具和庫，以提高開發效率和代碼質量。

**參考資料：**

- [Using C++ on Linux in VS Code](https://code.visualstudio.com/docs/cpp/config-linux)
- [How To Compile And Run a C/C++ Code In Linux - GeeksforGeeks](https://www.geeksforgeeks.org/how-to-compile-and-run-a-c-c-code-in-linux/)
- [Writing and Compiling C++ on Linux [A How-To Guide] - HackerNoon](https://hackernoon.com/writing-and-compiling-c-on-linux-a-how-to-guide-ddi032di)

*AI 生成時間：2025年2月18日，20:02:10*

---

### MSVC 編譯器命令行選項速查表

以下是 Microsoft Visual C++ (MSVC) 編譯器的常用命令行選項參考：

1. **基本命令：**
   - `cl <source_file>`: 編譯源文件
   - `cl /Fe<output_file> <source_file>`: 編譯並指定輸出文件名
   - `cl /c <source_file>`: 僅編譯不鏈接

2. **輸出文件：**
   - `/Fe<file>`: 命名可執行文件
   - `/Fo<file>`: 命名對象文件
   - `/Fd<file>`: 命名程序數據庫 (PDB) 文件

3. **調試：**
   - `/Zi`: 在 PDB 文件中啟用調試信息
   - `/Z7`: 在對象文件中啟用調試信息
   - `/DEBUG`: 在可執行文件中包含調試信息

4. **優化：**
   - `/O1`: 優化大小
   - `/O2`: 優化速度
   - `/Ox`: 完全優化
   - `/Ob<n>`: 內聯函數擴展 (n = 0, 1, 2)
   - `/Ot`: 偏向快速代碼

5. **預處理器：**
   - `/D<name>`: 定義宏
   - `/U<name>`: 取消定義宏
   - `/I<dir>`: 添加目錄到包含搜索路徑
   - `/P`: 預處理到文件（不編譯）

6. **鏈接：**
   - `/link`: 傳遞選項給鏈接器
   - `/LIBPATH:<dir>`: 添加目錄到庫搜索路徑
   - `/NODEFAULTLIB`: 忽略默認庫
   - `/OUT:<file>`: 指定輸出文件名
   - `/SUBSYSTEM:<subsystem>`: 指定子系統 (CONSOLE, WINDOWS)

7. **警告：**
   - `/W<n>`: 設置警告級別 (0-4)
   - `/WX`: 將警告視為錯誤
   - `/wd<n>`: 禁用特定警告
   - `/Wall`: 啟用所有警告
   - `/we<n>`: 將特定警告視為錯誤

8. **代碼生成：**
   - `/EHsc`: 啟用標準 C++ 異常處理
   - `/GR`: 啟用運行時類型信息 (RTTI)
   - `/MD`: 鏈接 MSVC 運行時庫 (DLL)
   - `/MT`: 鏈接 MSVC 運行時庫 (靜態)
   - `/openmp`: 啟用 OpenMP 支持
   - `/arch:<type>`: 指定架構 (SSE, SSE2)

9. **運行時檢查：**
   - `/RTC1`: 啟用運行時錯誤檢查
   - `/RTCs`: 啟用堆棧幀運行時錯誤檢查
   - `/RTCu`: 啟用未初始化變量運行時錯誤檢查

10. **其他：**
    - `/nologo`: 抑制啟動橫幅
    - `/MP`: 啟用多處理器編譯
    - `/analyze`: 啟用代碼分析
    - `/FS`: 強制同步 PDB 寫入
    - `/showIncludes`: 顯示包含文件路徑

11. **鏈接器選項：**
    - `/DLL`: 創建 DLL
    - `/INCREMENTAL`: 啟用增量鏈接
    - `/LTCG`: 啟用鏈接時代碼生成
    - `/MAP`: 生成映射文件
    - `/NOLOGO`: 抑制鏈接器啟動橫幅
    - `/OPT:<option>`: 鏈接器優化 (REF, ICF)
    - `/PDB:<file>`: 指定 PDB 文件名
    - `/RELEASE`: 在可執行文件頭中設置校驗和

12. **環境變量：**
    - `INCLUDE`: 包含文件的目錄
    - `LIB`: 庫文件的目錄
    - `PATH`: 可執行文件的目錄

使用 `cl /?` 或 `link /?` 命令獲取每個選項的更詳細信息。

*AI 生成內容，可能不完整或過時*

