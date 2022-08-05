#include "utils/Log.h"

#include <L4/web/Http.h>

namespace L4
{
    void Main()
    {
        LogSetup();
        Ensure<LogLevel::Critical>(false, "hello {:s}", [] { return std::make_format_args("world"); });
        auto Config = Web::Http::Get<Web::Json::Document>(cpr::Url { "https://clientconfig.rpg.riotgames.com/api/v1/config/public" });
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}