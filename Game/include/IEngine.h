#pragma once

//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Using-libraries-from-runtime-modifiable-classes
#include "RuntimeObjectSystem/RuntimeLinkLibrary.h"

RUNTIME_COMPILER_LINKLIBRARY("assimp-vc142-mtd.lib");
RUNTIME_COMPILER_LINKLIBRARY("luad.lib");
RUNTIME_COMPILER_LINKLIBRARY("imguid.lib");
RUNTIME_COMPILER_LINKLIBRARY("gladd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Engined.lib");
RUNTIME_COMPILER_LINKLIBRARY("tracyd.lib");
RUNTIME_COMPILER_LINKLIBRARY("glfw3d.lib");
RUNTIME_COMPILER_LINKLIBRARY("fmod_vc.lib");
RUNTIME_COMPILER_LINKLIBRARY("fmodstudio_vc.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet2FileLoaderd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Collisiond.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Commond.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Dynamicsd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Geometryd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3OpenCL_clewd.lib");
RUNTIME_COMPILER_LINKLIBRARY("BulletCollisiond.lib");
RUNTIME_COMPILER_LINKLIBRARY("BulletInverseDynamicsd.lib");
RUNTIME_COMPILER_LINKLIBRARY("BulletSoftBodyd.lib");
RUNTIME_COMPILER_LINKLIBRARY("LinearMathd.lib");

#ifdef _WIN32
RUNTIME_COMPILER_LINKLIBRARY("shell32.lib");//ShellExecuteA
RUNTIME_COMPILER_LINKLIBRARY("Comdlg32.lib");//FileOpenA etc
RUNTIME_COMPILER_LINKLIBRARY("gdi32.lib");//glfw
RUNTIME_COMPILER_LINKLIBRARY("Advapi32.lib");//tracy
#endif

//RUNTIME_COMPILER_LINKLIBRARY("fmodL_vc.lib");
//RUNTIME_COMPILER_LINKLIBRARY("fmodLibd.lib");
//RUNTIME_COMPILER_LINKLIBRARY("zlibstaticd.lib");