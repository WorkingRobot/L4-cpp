#pragma once

namespace L4
{
    template <class T, auto Destructor, T DefaultValue = nullptr>
    class RAIIWrapper
    {
    public:
        RAIIWrapper(T&& Handle) :
            Handle(std::move(Handle))
        {
        }

        RAIIWrapper(const RAIIWrapper&) = delete;

        RAIIWrapper(RAIIWrapper&& Other) noexcept :
            Handle(std::exchange(Other.Handle, DefaultValue))
        {
        }

        ~RAIIWrapper()
        {
            Destructor(Handle);
        }

        operator T() const noexcept
        {
            return Handle;
        }

    private:
        T Handle;
    };
}