#pragma once

#ifdef _WIN32
#ifdef L4_EXPORT
#define L4_API __declspec(dllexport)
#else
#define L4_API __declspec(dllimport)
#endif
#else
#define L4_API __attribute__((visibility("default")))
#endif

#ifdef _WIN32
#ifdef __clang__
#define L4_CLASS_API [[clang::lto_visibility_public]]
#else
#define L4_CLASS_API
#endif
#else
#define L4_CLASS_API __attribute__((visibility("default")))
#endif

#include <memory>

namespace L4::Interface
{
    class L4_CLASS_API ObjectBase : public std::enable_shared_from_this<ObjectBase>
    {
    protected:
        ObjectBase() = default;

    public:
        virtual ~ObjectBase() = default;
    };

    class L4_CLASS_API Interface : public ObjectBase
    {
    };

    template <class IntT = void>
    class L4_CLASS_API Object : public ObjectBase
    {
    public:
        using FI = IntT;
        using FIPtr = std::shared_ptr<IntT>;

    protected:
        Object(FIPtr Interface) :
            Interface(Interface)
        {
        }

        const FIPtr Interface;
    };

    template <>
    class L4_CLASS_API Object<void> : public ObjectBase
    {
    public:
        using FI = void;
        using FIPtr = std::nullptr_t;

    protected:
        Object(FIPtr Interface)
        {
        }
    };

    template<class T>
    using Ptr = std::weak_ptr<T>;

    template<class T>
    using OwningPtr = std::shared_ptr<T>;

    template<class T, class... ArgTs>
    static OwningPtr<T> CreateObject(ArgTs&&... Args)
    {
        return OwningPtr<T>(new T(std::forward<ArgTs>(Args)...));
    }

    template<class T>
    static OwningPtr<T> CreateObject()
    {
        return CreateObject<T>(nullptr);
    }

    template<class T>
    static OwningPtr<T> OwnedFromThis(T* Self)
    {
        return OwningPtr<T>(Self);
    }

    template <class T>
    static OwningPtr<const T> OwnedFromThis(const T* Self)
    {
        return OwningPtr<const T>(Self);
    }

    template <class T>
    static Ptr<T> UnownedFromThis(T* Self) noexcept
    {
        return Self;
    }

    template <class T>
    static Ptr<const T> UnownedFromThis(const T* Self) noexcept
    {
        return Self;
    }
}