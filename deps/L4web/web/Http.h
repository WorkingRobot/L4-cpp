#pragma once

#include "cpr.h"
#include "Json.h"
#include "Response.h"

#include <log/Log.h>

namespace L4::Web::Http
{
    namespace Detail
    {
        void InitializeSession(cpr::Session& Session);

        static constexpr auto ExtraCallbackDefault = [](cpr::Session& Session, auto&& Opt) {
            using T = std::decay_t<decltype(Opt)>;
            static_assert("Unknown option");
        };

        template <auto ExtraCallback, class... ExtraTs, class... ArgTs>
        void DecorateSession(cpr::Session& Session, ArgTs&&... Args)
        {
            InitializeSession(Session);

            static auto SetOption = [&Session]<class T>(T&& Opt) {
                if constexpr (std::disjunction_v<std::is_same<T, ExtraTs>...>)
                {
                    ExtraCallback(Session, std::forward<T>(Opt));
                }
                else
                {
                    Session.SetOption(std::forward<T>(Opt));
                }
            };
            // cpr::priv::set_option vvv
            std::initializer_list<int> ignore = { (SetOption(std::forward<ArgTs>(Args)), 0)... };
            (void)ignore;
            // cpr::priv::set_option ^^^
        }
    }

    template <class... ArgTs>
    static cpr::Response Delete(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return Session.Delete();
    }

    template <class T = std::string, Json::Encoding Enc = Json::Encoding::UTF8, class... ArgTs>
    static Response<T> Get(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return CreateResponse<T, Enc>(Session.Get());
    }

    template <class... ArgTs>
    static cpr::Response Head(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return Session.Head();
    }

    template <class... ArgTs>
    static cpr::Response Options(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return Session.Options();
    }

    template <class... ArgTs>
    static cpr::Response Patch(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return Session.Patch();
    }

    template <class... ArgTs>
    static cpr::Response Post(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return Session.Post();
    }

    template <class... ArgTs>
    static cpr::Response Put(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession<Detail::ExtraCallbackDefault>(Session, std::forward<ArgTs>(Args)...);
        return Session.Put();
    }
}