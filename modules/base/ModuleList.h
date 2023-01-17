#pragma once

#include "gui/Builder.h"
#include "log/Log.h"
#include "Module.h"
#include "storage/Store.h"
#include "utils/Stopwatch.h"

#include <type_name/type_name.hpp>

#include <memory>
#include <stdexcept>
#include <vector>

namespace L4::Modules
{
    class ModuleList
    {
    public:
        ModuleList();

        ~ModuleList();

        template <class T>
        T& GetModule()
        {
            for (auto& Module : Modules)
            {
                if (auto ModulePtr = dynamic_cast<T*>(Module.get()))
                {
                    return *ModulePtr;
                }
            }
            throw std::logic_error("Could not find module");
        }

        template <class T>
        [[nodiscard]] T& GetWidget(const Glib::ustring& Name) const
        {
            return Builder.GetWidget<T>(Name);
        }

        template <class T>
        [[nodiscard]] T GetStorage(const std::string_view Key)
        {
            return Store.Get<T>(Key);
        }

        template <class T>
        void SetStorage(const std::string_view Key, const T& Value)
        {
            return Store.Set(Key, Value);
        }

    protected:
        template <class T>
        static consteval std::string_view GetModuleNamespace()
        {
            auto Name = type_name_v<T>;
            std::string_view Begin = "class L4::Modules::";
            if (Name.starts_with(Begin))
            {
                Name.remove_prefix(Begin.size());
            }
            std::string_view End = "::Module";
            if (Name.ends_with(End))
            {
                Name.remove_suffix(End.size());
            }
            return Name;
        }

        template <class T>
        void AddModule()
        {
            auto Timer = Stopwatch::StartNew();
            const auto& Module = Modules.emplace_back(GetModuleNamespace<T>(), std::make_unique<T>(*this));
            Timer.Stop();
            LOG(Info, "{:s} module loaded in {:.2f} ms", Module.first, Timer.TimeElapsedMs());
        }

        Gui::Builder Builder;
        Storage::Store Store;
        std::vector<std::pair<std::string_view, std::unique_ptr<Base::Module>>> Modules;
    };
}