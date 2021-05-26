# Campfire
![Ubuntu build](https://github.com/danhuynh0803/Campfire/workflows/Ubuntu%20build/badge.svg)
![Windows build](https://github.com/danhuynh0803/Campfire/workflows/Windows%20build/badge.svg)

## Summary
Campfire is an in-progress game engine built using OpenGL and Vulkan. Most of the engine's features are currently disabled, as we work through switching to Vulkan, but are available on the [OpenGL branch](https://github.com/danhuynh0803/Campfire/tree/OpenGL).

## Sample Images
![SampleScene](https://i.imgur.com/jZDVC6l.jpg)

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
