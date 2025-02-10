# Campfire
![Ubuntu build](https://github.com/danhuynh0803/Campfire/workflows/Ubuntu%20build/badge.svg)
![Windows build](https://github.com/danhuynh0803/Campfire/workflows/Windows%20build/badge.svg)

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
