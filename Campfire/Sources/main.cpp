#include "Core/Application.h"
#include "Core/Log.h"

int main(int argc, char * argv[])
{
    Log::Start();

    Application app;

    app.Run();

    return 0;
}
