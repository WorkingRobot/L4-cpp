#include "Module.h"

#include "utils/Config.h"

namespace L4::Modules::Plugins
{
    Module::Module(ModuleList& Ctx) :
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
            auto Filename = Entry.path().filename().string();
            if (Filename.starts_with("L4-plugin-") && Filename.ends_with(".dll"))
            {
                auto& Plugin = Plugins.emplace_back(Entry);
                Plugin.OnAuthUserUpdated = []() {};
                Plugin.Initialize(Client);
            }
        }
    }
}