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

        template <class... ArgTs>
        void DecorateSession(cpr::Session& Session, ArgTs&&... Args)
        {
            InitializeSession(Session);

            auto SetOption = [&Session, AlreadyProcessedHeaders = false](auto&& Arg) mutable {
                if constexpr (std::convertible_to<decltype(Arg), cpr::Header>)
                {
                    if (AlreadyProcessedHeaders)
                    {
                        Session.UpdateHeader(std::forward<decltype(Arg)>(Arg));
                        return;
                    }
                    else
                    {
                        AlreadyProcessedHeaders = true;
                    }
                }

                Session.SetOption(std::forward<decltype(Arg)>(Arg));
            };

            (SetOption(std::forward<ArgTs>(Args)), ...);
        }
    }

    template <class... ArgTs>
    static cpr::Response Delete(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return Session.Delete();
    }

    template <class T = std::string, int SuccessHttpCode = -1, Json::Encoding Enc = Json::Encoding::UTF8, class... ArgTs>
    static Response<T> Get(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return CreateResponse<T, SuccessHttpCode, Enc>(Session.Get());
    }

    template <class... ArgTs>
    static cpr::Response Head(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return Session.Head();
    }

    template <class... ArgTs>
    static cpr::Response Options(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return Session.Options();
    }

    template <class... ArgTs>
    static cpr::Response Patch(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return Session.Patch();
    }

    template <class T = std::string, int SuccessHttpCode = -1, Json::Encoding Enc = Json::Encoding::UTF8, class... ArgTs>
    static Response<T> Post(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return CreateResponse<T, SuccessHttpCode, Enc>(Session.Post());
    }

    template <class... ArgTs>
    static cpr::Response Put(ArgTs&&... Args)
    {
        cpr::Session Session;
        Detail::DecorateSession(Session, std::forward<ArgTs>(Args)...);
        return Session.Put();
    }
}