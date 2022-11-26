#pragma once

#include "compat/expected.h"
#include "cpr.h"
#include "format/Format.h"
#include "Json.h"

#include <string>

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

    struct ResponseError
    {
        ResponseStatus Status;
        std::string Error;
    };

    template <class T>
    using Response = std::expected<T, ResponseError>;

    template <class T, Json::Encoding Enc = Json::Encoding::UTF8>
    static Response<T> CreateResponse(cpr::Response&& Resp)
    {
        if (Resp.status_code < 200 || Resp.status_code >= 300)
        {
            return Response<T>(std::unexpect, ResponseStatus::BadHttpCode, std::to_string(Resp.status_code));
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
                return Response<T>(std::unexpect, ResponseStatus::InvalidFormat, FMT::format("{:d} @ {:d}", (int)Document.GetParseError(), Document.GetErrorOffset()));
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
                    return Response<T>(std::unexpect, ResponseStatus::BadFormat, "");
                }
            }
        }
    }
}