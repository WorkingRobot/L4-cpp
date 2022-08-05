#include "utils/Log.h"

#include "source/SourceManager.h"

namespace L4
{
    void Main()
    {
        LogSetup();

        SourceManager Manager;
        Manager.Load(R"(J:\Code\Projects\L4\builds\ninja-multi-vcpkg\sources\riot\Debug\SourceRiot.dll)");
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}