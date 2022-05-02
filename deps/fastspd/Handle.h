#pragma once

namespace FastSpd
{
    template <auto Deleter, class T, auto InvalidValue = ~0llu>
    class Handle
    {
    public:
        Handle() noexcept :
            Internal(InvalidValue)
        {
        }

        Handle(T Handle) noexcept :
            Internal(Handle)
        {
        }

        ~Handle() noexcept(noexcept(Deleter))
        {
            Deleter(Internal);
        }

        Handle(Handle&& Other) noexcept :
            Internal(InvalidValue)
        {
            Other.swap(*this);
        }

        Handle& operator=(Handle&& Other) noexcept
        {
            Other.swap(*this);
            return *this;
        }

        Handle(const Handle&) = delete;
        Handle& operator=(const Handle&) = delete;

        T get() const noexcept
        {
            return Internal;
        }

        operator T() const noexcept
        {
            return Internal;
        }

        explicit operator bool() const noexcept
        {
            return Internal != T(InvalidValue);
        }

        T release() noexcept
        {
            T Ret = InvalidValue;
            std::swap(Ret, Internal);
            return Ret;
        }

        void swap(Handle& Other) noexcept
        {
            std::swap(Internal, Other.Internal);
        }

        void reset() noexcept(noexcept(Deleter))
        {
            T Tmp = release();
            Deleter(Tmp);
        }

    private:
        T Internal;
    };
}