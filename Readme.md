# Campfire
![](enginePlayState.gif)

## Summary
Campfire is an in-progress game engine intended for use in small gamejam projects. Not currently usable other than for myself due to a lot of hardcoded file paths for testing purposes. Campfire uses [Glitter](https://github.com/Polytonic/Glitter) as the starting template but requires at least C++17 due to certain imgui widgets.

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

# Mac OSX
cmake -G "Xcode" ..

# Microsoft Windows
cmake -G "Visual Studio 16 2019" ..
```

Functionality           | Library
----------------------- | ------------------------------------------
Mesh Loading            | [assimp](https://github.com/assimp/assimp)
Physics                 | [bullet](https://github.com/bulletphysics/bullet3)
OpenGL Function Loader  | [glad](https://github.com/Dav1dde/glad)
Windowing and Input     | [glfw](https://github.com/glfw/glfw)
OpenGL Mathematics      | [glm](https://github.com/g-truc/glm)
Texture Loading         | [stb](https://github.com/nothings/stb)
Logging                 | [spdlog](https://github.com/gabime/spdlog)
Scene Loading/Saving    | [rapidjson](https://github.com/Tencent/rapidjson)
GUI                     | [imgui](https://github.com/ocornut/imgui)
