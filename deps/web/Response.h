#pragma once

#include "compat/expected.h"
#include "cpr.h"
#include "format/Format.h"
#include "Json.h"

#include <rapidjson/error/en.h>

#include <string>

namespace L4::Web::Http
{
    enum class ErrorType : uint8_t
    {
        BadHttpCode,
        BadCurl,
        BadJson,
        BadFormat
    };

    struct Error
    {
        ErrorType Type;
        std::string Message;
    };

    template <class T>
    using Response = std::expected<T, Error>;

    template <class T, int SuccessHttpCode = -1, Json::Encoding Enc = Json::Encoding::UTF8>
    static Response<T> CreateResponse(cpr::Response&& Resp)
    {
        if (Resp.error)
        {
            return Response<T>(std::unexpect, Error { ErrorType::BadCurl, fmt::format("{:s} ({:d})", std::move(Resp.error.message), std::to_underlying(Resp.error.code)) });
        }

        if constexpr (SuccessHttpCode == -1)
        {
            if (Resp.status_code < 200 || Resp.status_code >= 300) [[unlikely]]
            {
                return Response<T>(std::unexpect, Error { ErrorType::BadHttpCode, std::to_string(Resp.status_code) });
            }
        }
        else
        {
            if (Resp.status_code != SuccessHttpCode) [[unlikely]]
            {
                return Response<T>(std::unexpect, Error { ErrorType::BadHttpCode, std::to_string(Resp.status_code) });
            }
        }

        // void
        if constexpr (std::is_void_v<T>)
        {
            return Response<T>();
        }
        // cpr::Response
        else if constexpr (std::is_same_v<T, decltype(Resp)>)
        {
            return Response<T>(std::move(Resp));
        }
        // std::string
        else if constexpr (std::is_same_v<T, decltype(Resp.text)>)
        {
            return Response<T>(std::move(Resp.text));
        }
        // Json::Document or parsable
        else if constexpr (std::is_same_v<T, Json::DocumentBase<Enc>> || Json::Parsable<T>)
        {
            auto Document = Json::Parse<Enc>({ (const Json::Detail::JsonCharT<Enc>*)Resp.text.c_str(), Resp.text.size() / sizeof(Json::Detail::JsonCharT<Enc>) });
            rapidjson::ParseResult ParseResult = Document;
            if (!ParseResult)
            {
                return Response<T>(std::unexpect, Error { ErrorType::BadJson, fmt::format("{:s} @ {:d}", rapidjson::GetParseError_En(ParseResult.Code()), ParseResult.Offset()) });
            }
            if constexpr (std::is_same_v<T, Json::DocumentBase<Enc>>)
            {
                return Response<T>(std::move(Document));
            }
            else if constexpr (Json::Parsable<T>)
            {
                T Object {};
                try
                {
                    Json::Parser<T> {}(Document, Object);
                }
                catch (const Json::ParseException& Exception)
                {
                    return Response<T>(std::unexpect, Error { ErrorType::BadFormat, Exception.what() });
                }
                return Response<T>(std::move(Object));
            }
        }
        // Constructible from std::string
        else
        {
            return Response<T>(std::move(Resp.text));
        }
    }
}