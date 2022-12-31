#include "Module.h"

#include "utils/Config.h"

namespace L4::Modules::Plugins
{
    Module::Module() :
        Client({
                   .Id = u8"l4",
                   .Name = u8"L4",
                   .Version = (char8_t*)Config::GetVersionLong(),
                   .VersionNumeric = Config::GetVersionNumeric(),
               },
               Config::GetFolder())
    {
        for (auto& Entry : std::filesystem::directory_iterator(R"(J:\Code\Projects\L4\builds\msvc\lib\Debug)"))
        {
            if (Entry.path().filename().string().starts_with("L4-plugin-"))
            {
                auto& Plugin = Plugins.emplace_back(Entry);
                Plugin.OnAuthUserUpdated = []() {};
                Plugin.Initialize(Client);
            }
        }
    }
}