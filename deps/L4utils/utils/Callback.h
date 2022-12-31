#pragma once

#include <functional>

namespace L4
{
    template <class T>
    class Callback
    {
    public:
        Callback() requires(std::is_void_v<typename std::move_only_function<T>::result_type>) :
            Function([]() {})
        {
        }

        Callback(auto&& Func) :
            Function(std::forward<decltype(Func)>(Func))
        {
        }

        void Set(auto&& Func)
        {
            Function = std::forward<decltype(Func)>(Func);
        }

        auto operator()(auto&&... Args)
        {
            return Function(std::forward<decltype(Args)>(Args)...);
        }

    private:
        std::move_only_function<T> Function;
    };
}