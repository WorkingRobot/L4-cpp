#pragma once

#include "gui/Builder.h"
#include "Module.h"
#include "storage/Store.h"

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
        void AddModule()
        {
            Modules.emplace_back(std::make_unique<T>(*this));
        }

        Gui::Builder Builder;
        Storage::Store Store;
        std::vector<std::unique_ptr<Base::Module>> Modules;
    };
}