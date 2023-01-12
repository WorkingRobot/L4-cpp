#pragma once

#include <memory>

namespace L4
{
    // This allows any class to be moved, but keep a constant pointer to the true instance.
    // If we pass a T* to the unique pointer, we'll dynamically update the T* and use the
    // unique pointer externally as a T**. This adds a level of indirection, but any passed T**
    // will stay valid for as long as the true instance is still in scope and the unique pointer
    // does not release its memory.
    template <class T>
    class EnableSelfUniqueFromThis
    {
    protected:
        explicit EnableSelfUniqueFromThis() :
            SelfPtr(std::make_unique<T*>(static_cast<T*>(this)))
        {
        }

        EnableSelfUniqueFromThis(const EnableSelfUniqueFromThis& Other) = delete;
        EnableSelfUniqueFromThis& operator=(const EnableSelfUniqueFromThis& Other) noexcept = delete;

        EnableSelfUniqueFromThis(EnableSelfUniqueFromThis&& Other) noexcept :
            SelfPtr(std::exchange(Other.SelfPtr, nullptr))
        {
            if (SelfPtr)
            {
                *SelfPtr = static_cast<T*>(this);
            }
        }

        EnableSelfUniqueFromThis& operator=(EnableSelfUniqueFromThis&& Other) noexcept
        {
            std::swap(SelfPtr, Other.SelfPtr);

            if (SelfPtr)
            {
                *SelfPtr = static_cast<T*>(this);
            }

            return *this;
        }

    public:
        // Only valid if not moved from. GetSelfPtr() should be called before being moved.
        [[nodiscard]] T** GetSelfPtr() const noexcept
        {
            return SelfPtr.get();
        }

        [[nodiscard]] bool OwnsSelf() const noexcept
        {
            if (!SelfPtr)
            {
                return false;
            }
            if (*SelfPtr != static_cast<const T*>(this))
            {
                return false;
            }

            return true;
        }

    private:
        std::unique_ptr<T*> SelfPtr;
    };
}