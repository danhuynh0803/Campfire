project(NightOwl LANGUAGES CSharp)

# Include CMake utilities for CSharp, for WinForm support.
include(CSharpUtilities)

add_executable(NightOwl
    #App.config
    Form1.cs
    Form1.Designer.cs
    Program.cs
    #Properties/AssemblyInfo.cs
    #Properties/Resources.Designer.cs
    #Properties/Resources.resx
    #Properties/Settings.Designer.cs
    #Properties/Settings.settings
)

# Set the .NET Framework version for the executable.
set_property(TARGET NightOwl PROPERTY DOTNET_TARGET_FRAMEWORK "net5.0-windows")
# Set the executable to be 32-bit.
set_property(TARGET NightOwl PROPERTY WIN32_EXECUTABLE TRUE)
# Set the C# language version
set(CMAKE_CSharp_FLAGS "/langversion:latest")

# Set the source file properties for Windows Forms use.
csharp_set_windows_forms_properties(
    Form1.cs
    Form1.Designer.cs
    Program.cs
    #Properties/AssemblyInfo.cs
    #Properties/Resources.Designer.cs
    #Properties/Resources.resx
    #Properties/Settings.Designer.cs
    #Properties/Settings.settings
)

#target_link_libraries(NightOwl MyLocalLib)

#set_property(TARGET NightOwl PROPERTY 
#    VS_DOTNET_REFERENCE_MyThirdPartyLib /path/to/libs/MyThirdPartyLib.dll)

# Add in the .NET reference libraries.
set_property(TARGET NightOwl PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "System"
    "System.Core"
    "System.Data"
    "System.Windows.Forms"
)