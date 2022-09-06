#include "log/Debug.h"
#include "log/Log.h"
#include "source/SourceManager.h"

namespace L4
{
    void Main()
    {
        SetupLogging();
        Debug::SetupExceptions();

        SourceManager Manager;
        Manager.Load(R"(J:\Code\Projects\L4\builds\clang-cl\lib\Debug\L4-source-riot.dll)");
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}