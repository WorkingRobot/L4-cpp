#pragma once

#include <stdint.h>

// prevents windows.h from being included
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
typedef uintptr_t SOCKET;
struct fd_set;
struct sockaddr {
    uint16_t sa_family;
    char sa_data[14];
};
#endif
#include <cpr/session.h>

namespace L4::Http {
    namespace Detail {
        cpr::Session CreateSession();

        static constexpr auto ExtraCallbackDefault = [](cpr::Session& Session, auto&& Opt) {
            using T = std::decay_t<decltype(Opt)>;
            if constexpr (std::is_same_v<T, ReserveSize>) {
                Session.ResponseStringReserve(Opt.size);
            } else {
                static_assert("Unknown option");
            }
        };

        template <auto ExtraCallback, class... ExtraTs, class... ArgTs>
        cpr::Session GetSession(ArgTs&&... Args)
        {
            cpr::Session Session = CreateSession();
            // Marking this vvv static literally causes an internal compiler error.
            constexpr auto SetOption = [&Session]<class T>(T&& Opt) {
                if constexpr (std::disjunction_v<std::is_same<T, ExtraTs>...>) {
                    ExtraCallback(Session, std::forward<T>(Opt));
                } else {
                    Session.SetOption(std::forward<T>(Opt));
                }
            };
            // cpr::priv::set_option vvv
            std::initializer_list<int> ignore = { (SetOption(std::forward<ArgTs>(Args)), 0)... };
            (void)ignore;
            // cpr::priv::set_option ^^^
            return Session;
        }
    }

    class ReserveSize {
    public:
        size_t size = 0;

        ReserveSize() = default;
        explicit ReserveSize(size_t _size) :
            size(_size)
        {
        }
    };

    template <class... ArgTs>
    static cpr::Response Delete(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Delete();
    }

    template <class... ArgTs>
    static cpr::Response Get(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Get();
    }

    template <class... ArgTs>
    static cpr::Response Head(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Head();
    }

    template <class... ArgTs>
    static cpr::Response Options(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Options();
    }

    template <class... ArgTs>
    static cpr::Response Patch(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Patch();
    }

    template <class... ArgTs>
    static cpr::Response Post(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Post();
    }

    template <class... ArgTs>
    static cpr::Response Put(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Put();
    }
}