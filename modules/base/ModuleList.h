#pragma once

#include "Module.h"

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

    private:
        template<class T>
        void AddModule();

        std::vector<std::unique_ptr<Base::Module>> Modules;
    };
}