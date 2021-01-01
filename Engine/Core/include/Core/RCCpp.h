// Wrapper class around RuntimeCompiledCpp

#include <RuntimeObjectSystem/RuntimeObjectSystem.h>

class RCCpp
{
public:
    static bool Init();
    static void Update(float dt);
    static void Shutdown();

private:
    static IRuntimeObjectSystem* s_pRuntimeObjectSystem;
    //static StdioLogSystem           g_Logger;
};
