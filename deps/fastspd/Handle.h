#pragma once

namespace FastSpd
{
    template <auto Deleter, class T>
    class Handle
    {
    public:
        Handle() noexcept :
            Internal()
        {
        }

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        Handle(T Handle) noexcept :
            Internal(Handle)
        {
        }

        ~Handle() noexcept(noexcept(Deleter))
        {
            Deleter(Internal);
        }

        Handle(Handle&& Other) noexcept :
            Internal()
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

        [[nodiscard]] T get() const noexcept
        {
            return Internal;
        }

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        operator T() const noexcept
        {
            return Internal;
        }

        T release() noexcept
        {
            T Ret();
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