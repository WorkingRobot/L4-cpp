#pragma once

#include "Json.h"

#include <format>

// Prevents Windows.h from being included
#if defined(_WIN32) && !defined(_WINSOCKAPI_)
#define _WINSOCKAPI_
typedef uintptr_t SOCKET;
struct fd_set;
struct sockaddr
{
    uint16_t sa_family;
    char sa_data[14];
};
#endif
#include <cpr/session.h>

namespace L4::Web::Http
{
    enum class ResponseStatus : uint8_t
    {
        Success,
        BadHttpCode,
        Cancelled,
        Unauthorized,
        InvalidFormat,
        BadFormat
    };

    template <class T>
    class Response
    {
    public:
        explicit Response(T&& Object) :
            Status(ResponseStatus::Success),
            Object(std::move(Object))
        {
        }

        explicit Response(ResponseStatus Status, std::string Error) :
            Status(Status),
            Error(std::move(Error))
        {
            // TODO: Ensure Status != ResponseStatus::Success
        }

        Response(const Response& Other) :
            Status(Other.Status)
        {
            if (Status == ResponseStatus::Success)
            {
                Object = Other.Object;
            }
            else
            {
                Error = Other.Error;
            }
        }

        Response& operator=(const Response& Other)
        {
            if (this == &Other)
            {
                return *this;
            }

            Status = Other.Status;
            if (Status == ResponseStatus::Success)
            {
                Object = Other.Object;
            }
            else
            {
                Error = Other.Error;
            }
            return *this;
        }

        Response(Response&& Other) noexcept :
            Status(Other.Status)
        {
            if (Status == ResponseStatus::Success)
            {
                Object = std::exchange(Other.Object, {});
            }
            else
            {
                Error = std::exchange(Other.Error, {});
            }
        }

        Response& operator=(Response&& Other) noexcept
        {
            if (this == &Other)
            {
                return *this;
            }

            Status = Other.Status;
            if (Status == ResponseStatus::Success)
            {
                Object = std::exchange(Other.Object, {});
            }
            else
            {
                Error = std::exchange(Other.Error, {});
            }
            return *this;
        }

        ~Response()
        {
            if (Status == ResponseStatus::Success)
            {
                Object.~T();
            }
            else
            {
                Error.~basic_string();
            }
        }

        explicit operator bool() const noexcept
        {
            return Status == ResponseStatus::Success;
        }

        ResponseStatus GetStatus() const noexcept
        {
            return Status;
        }

        const T& Get() const
        {
            return Object;
        }

        T& Get()
        {
            return Object;
        }

        const T* operator->() const
        {
            return &Object;
        }

        T* operator->()
        {
            return &Object;
        }

    private:
        ResponseStatus Status;
        union
        {
            T Object;
            std::string Error;
        };
    };

    template <>
    class Response<void>
    {
    public:
        explicit Response() :
            Status(ResponseStatus::Success)
        {
        }

        explicit Response(ResponseStatus Status, std::string Error) :
            Status(Status),
            Error(std::move(Error))
        {
            // TODO: Ensure Status != ResponseStatus::Success
        }

        explicit operator bool() const noexcept
        {
            return Status == ResponseStatus::Success;
        }

        ResponseStatus GetStatus() const noexcept
        {
            return Status;
        }

    private:
        ResponseStatus Status;
        std::string Error;
    };

    template <class T, Json::Encoding Enc = Json::Encoding::UTF8>
    static Response<T> CreateResponse(cpr::Response&& Resp)
    {
        if (Resp.status_code < 200 || Resp.status_code >= 300)
        {
            return Response<T>(ResponseStatus::BadHttpCode, std::to_string(Resp.status_code));
        }

        if constexpr (std::is_void_v<T>)
        {
            return Response<T>();
        }
        else if constexpr (std::is_same_v<T, decltype(Resp.text)>)
        {
            return Response<T>(std::move(Resp.text));
        }
        else
        {
            auto Document = Json::Parse<Enc>({ (const Json::Detail::JsonCharT<Enc>*)Resp.text.c_str(), Resp.text.size() / sizeof(Json::Detail::JsonCharT<Enc>) });
            if (Document.HasParseError())
            {
                return Response<T>(ResponseStatus::InvalidFormat, std::format("{:d} @ {:d}", (int)Document.GetParseError(), Document.GetErrorOffset()));
            }
            if constexpr (std::is_same_v<decltype(Document), T>)
            {
                return Response<T>(std::move(Document));
            }
            else
            {
                T Object {};
                if (Json::Parser<T> {}(Document, Object))
                {
                    return Response<T>(std::move(Object));
                }
                else
                {
                    return Response<T>(ResponseStatus::BadFormat, "");
                }
            }
        }
    }

    class ReserveSize
    {
    public:
        size_t size = 0;

        ReserveSize() = default;
        explicit ReserveSize(size_t _size) :
            size(_size)
        {
        }
    };

    namespace Detail
    {
        cpr::Session CreateSession();

        static constexpr auto ExtraCallbackDefault = [](cpr::Session& Session, auto&& Opt) {
            using T = std::decay_t<decltype(Opt)>;
            if constexpr (std::is_same_v<T, ReserveSize>)
            {
                Session.ResponseStringReserve(Opt.size);
            }
            else
            {
                static_assert("Unknown option");
            }
        };

        template <auto ExtraCallback, class... ExtraTs, class... ArgTs>
        cpr::Session GetSession(ArgTs&&... Args)
        {
            cpr::Session Session = CreateSession();
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
            return Session;
        }
    }

    template <class... ArgTs>
    static cpr::Response Delete(ArgTs&&... Args)
    {
        return Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Delete();
    }

    template <class T = std::string, Json::Encoding Enc = Json::Encoding::UTF8, class... ArgTs>
    static Response<T> Get(ArgTs&&... Args)
    {
        return CreateResponse<T, Enc>(Detail::GetSession<Detail::ExtraCallbackDefault, Http::ReserveSize>(std::forward<ArgTs>(Args)...).Get());
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