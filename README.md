# Campfire
![Ubuntu build](https://github.com/danhuynh0803/Campfire/workflows/Ubuntu%20build/badge.svg)
![Windows build](https://github.com/danhuynh0803/Campfire/workflows/Windows%20build/badge.svg)
![CodeQL Advanced](https://github.com/danhuynh0803/Campfire/actions/workflows/codeql.yml/badge.svg)

## Summary
Campfire is an in-progress game engine built using OpenGL and Vulkan. Most of the engine's features are currently disabled, as we work through switching to Vulkan, but are available on the [OpenGL branch](https://github.com/danhuynh0803/Campfire/tree/OpenGL).

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

### Portions worked on
- Particle Effect System
- File system call
- Lua Scripting

### Key features and improvements
- Helping and debugging the Vulkan Layer for rendering using Vulkan API.

## 要在 Linux 上进行 C++ 编程

要在 Linux 上进行 C++ 编程，您需要安装编译器、设置开发环境，并熟悉基本的编译和调试过程。以下是详细步骤：

1. **安装编译器：**
    大多数 Linux 发行版默认包含 GNU 编译器集合 (GCC)。 您可以通过以下命令检查是否已安装： 

    ```bash
   gcc -v
   ```


    如果未安装，请使用以下命令进行安装：  

   - **Debian/Ubuntu：**
      ```bash
     sudo apt-get update
     sudo apt-get install build-essential gdb
     ```  

   - **Red Hat/Fedora：**
      ```bash
     sudo dnf group install "Development Tools"
     ```  

    这些命令将安装 GCC 编译器和 GDB 调试器。  

2. **编写 C++ 程序：**
    使用您喜欢的文本编辑器（如 Vim、Nano 或 Visual Studio Code）创建一个新的 C++ 源文件。例如，使用 Nano 创建一个名为 `hello.cpp` 的文件：  

    ```bash
   nano hello.cpp
   ```


    在文件中输入以下代码：  

    ```cpp
   #include <iostream>

   int main() {
       std::cout << "Hello, World!" << std::endl;
       return 0;
   }
   ```


    保存并关闭编辑器。  

3. **编译程序：**
    在终端中，导航到包含 `hello.cpp` 的目录，并使用以下命令编译：  

    ```bash
   g++ -o hello hello.cpp
   ```


    此命令使用 `g++` 编译器将源代码编译为名为 `hello` 的可执行文件。  

4. **运行程序：**
    编译成功后，使用以下命令运行程序：  

    ```bash
   ./hello
   ```


    您应会在终端中看到输出：`Hello, World!`。  

5. **使用集成开发环境 (IDE)：**
    虽然命令行工具功能强大，但使用 IDE 可以提高开发效率。  Visual Studio Code 是一个流行的选择。  您可以通过以下步骤在 Linux 上设置 Visual Studio Code 进行 C++ 开发：  

   - **安装 Visual Studio Code：**
      从 [Visual Studio Code 官方网站](https://code.visualstudio.com/) 下载适用于您发行版的安装包，并按照指示进行安装。  

   - **安装 C++ 扩展：**
      启动 Visual Studio Code，点击左侧的扩展图标（或按 `Ctrl+Shift+X`），搜索 `C++`，然后安装由 Microsoft 提供的 C/C++ 扩展。  

   - **配置编译和调试环境：**
      按照 [Visual Studio Code 的官方文档](https://code.visualstudio.com/docs/cpp/config-linux) 中的指南，设置 `tasks.json` 和 `launch.json` 以配置编译和调试任务。  

6. **调试程序：**
    使用 GDB 调试器，您可以在终端中运行以下命令启动调试：  

    ```bash
   gdb ./hello
   ```


    在 GDB 提示符下，输入 `run` 运行程序，使用 `break` 设置断点，使用 `step` 或 `next` 单步执行代码。  

通过以上步骤，您可以在 Linux 上成功进行 C++ 开发。随着经验的积累，您可以探索更多高级工具和库，以提高开发效率和代码质量。

**参考资料：**

- [Using C++ on Linux in VS Code](https://code.visualstudio.com/docs/cpp/config-linux)
- [How To Compile And Run a C/C++ Code In Linux - GeeksforGeeks](https://www.geeksforgeeks.org/how-to-compile-and-run-a-c-c-code-in-linux/)
- [Writing and Compiling C++ on Linux [A How-To Guide] - HackerNoon](https://hackernoon.com/writing-and-compiling-c-on-linux-a-how-to-guide-ddi032di)

*生成时间：2025年2月18日，20:02:10* 

=======

## Contributions by ewdlop

### Portions worked on
- Editor Layer
- Vulkan Layer
- Hierarchy Widget
- Inspector Widget
- Transform Widget
- Asset Browser

### Key features and improvements
- Implemented the Editor Layer with various functionalities such as scene management, user input processing, and rendering.

## 
MSVC Compiler Command Line Options Cheat Sheet
==============================================

1. **Basic Commands:**
   - `cl <source_file>`: Compile a source file.
   - `cl /Fe<output_file> <source_file>`: Compile and specify the output file name.
   - `cl /c <source_file>`: Compile without linking.

2. **Output Files:**
   - `/Fe<file>`: Name the executable file.
   - `/Fo<file>`: Name the object file.
   - `/Fd<file>`: Name the program database (PDB) file.

3. **Debugging:**
   - `/Zi`: Enable debugging information in PDB file.
   - `/Z7`: Enable debugging information in the object file.
   - `/DEBUG`: Include debug information in the executable.

4. **Optimization:**
   - `/O1`: Optimize for size.
   - `/O2`: Optimize for speed.
   - `/Ox`: Full optimization.
   - `/Ob<n>`: Inline function expansion (n = 0, 1, 2).
   - `/Ot`: Favor fast code.

5. **Preprocessor:**
   - `/D<name>`: Define a macro.
   - `/U<name>`: Undefine a macro.
   - `/I<dir>`: Add directory to include search path.
   - `/P`: Preprocess to a file (no compilation).

6. **Linking:**
   - `/link`: Pass options to the linker.
   - `/LIBPATH:<dir>`: Add directory to library search path.
   - `/NODEFAULTLIB`: Ignore default libraries.
   - `/OUT:<file>`: Specify name of the output file.
   - `/SUBSYSTEM:<subsystem>`: Specify the subsystem (CONSOLE, WINDOWS).

7. **Warnings:**
   - `/W<n>`: Set warning level (0-4).
   - `/WX`: Treat warnings as errors.
   - `/wd<n>`: Disable specific warning.
   - `/Wall`: Enable all warnings.
   - `/we<n>`: Treat specific warning as error.

8. **Code Generation:**
   - `/EHsc`: Enable standard C++ exception handling.
   - `/GR`: Enable runtime type information (RTTI).
   - `/MD`: Link with MSVC runtime library (DLL).
   - `/MT`: Link with MSVC runtime library (static).
   - `/openmp`: Enable OpenMP support.
   - `/arch:<type>`: Specify architecture (SSE, SSE2).

9. **Runtime Checks:**
   - `/RTC1`: Enable runtime error checks.
   - `/RTCs`: Enable stack frame runtime error checks.
   - `/RTCu`: Enable uninitialized variable runtime error checks.

10. **Miscellaneous:**
    - `/nologo`: Suppress startup banner.
    - `/MP`: Enable multi-processor compilation.
    - `/analyze`: Enable code analysis.
    - `/FS`: Force synchronous PDB writes.
    - `/showIncludes`: Show include file paths.

11. **Linker Options:**
    - `/DLL`: Create a DLL.
    - `/INCREMENTAL`: Enable incremental linking.
    - `/LTCG`: Enable link-time code generation.
    - `/MAP`: Generate a map file.
    - `/NOLOGO`: Suppress linker startup banner.
    - `/OPT:<option>`: Linker optimizations (REF, ICF).
    - `/PDB:<file>`: Specify name of the PDB file.
    - `/RELEASE`: Set the checksum in the header of the executable.

12. **Environment Variables:**
    - `INCLUDE`: Directories for include files.
    - `LIB`: Directories for library files.
    - `PATH`: Directories for executable files.

Use the `/help` option with `cl` or `link` commands for more detailed information about each option.
- Developed the Vulkan Layer for rendering using Vulkan API.
- Created the Hierarchy Widget for displaying and managing the scene hierarchy.
- Built the Inspector Widget for inspecting and modifying entity properties.
- Designed the Transform Widget for manipulating entity transforms.
- Developed the Asset Browser for browsing and managing assets.

