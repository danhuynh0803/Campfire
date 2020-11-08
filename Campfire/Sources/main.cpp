#ifdef Lua
#include "Scripting/ScriptEngine.h"
#else
#include "Core/Application.h"
#include "Scripting/ArenaAllocator.h"
#endif

int main(int argc, char * argv[])
{
#ifdef Lua
    ScriptEngine engine;
#else
    Application app;
    app.Run();
#endif // Lua
    return 0;
}
