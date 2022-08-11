#pragma once

#include "Json.h"

#ifndef JSON_ERROR_PARSE
#define JSON_ERROR_PARSE return false;
#endif

#ifndef JSON_ERROR_NOTFOUND
#define JSON_ERROR_NOTFOUND return false;
#endif

#ifndef JSON_STRING_PREFIX
#define JSON_STRING_PREFIX u8
#endif

#define JSON_STRING2(a, b) a##b
#define JSON_STRING1(a, b) JSON_STRING2(a, b)
#define JSON_STRING(str) JSON_STRING1(JSON_STRING_PREFIX, str)

#define JSON_STRING_TYPE ::std::JSON_STRING(string)

using JsonString = JSON_STRING_TYPE;

#define JSON_DEFINE                                \
    bool Parse(const ::L4::Web::Json::Value& Json) \
    {                                              \
        ::L4::Web::Json::Value::ConstMemberIterator Itr;

#define JSON_DEFINE_END \
    return true;        \
    }

#define JSON_BASE(BaseClass)     \
    if (!BaseClass::Parse(Json)) \
    {                            \
        JSON_ERROR_PARSE;        \
    }

#define JSON_ITEM(JsonName, TargetVariable)                                                \
    Itr = Json.FindMember(JSON_STRING(JsonName));                                          \
    if (Itr == Json.MemberEnd())                                                           \
    {                                                                                      \
        JSON_ERROR_NOTFOUND;                                                               \
    }                                                                                      \
    if (!::L4::Web::Json::Parser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
    {                                                                                      \
        JSON_ERROR_PARSE;                                                                  \
    }

#define JSON_ITEM_NULL(JsonName, TargetVariable)                                                                   \
    Itr = Json.FindMember(JSON_STRING(JsonName));                                                                  \
    if (Itr == Json.MemberEnd())                                                                                   \
    {                                                                                                              \
        JSON_ERROR_NOTFOUND;                                                                                       \
    }                                                                                                              \
    if (!Itr->value.IsNull() && !::L4::Web::Json::Parser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
    {                                                                                                              \
        JSON_ERROR_PARSE;                                                                                          \
    }

#define JSON_ITEM_OPT(JsonName, TargetVariable)                                                \
    Itr = Json.FindMember(JSON_STRING(JsonName));                                              \
    if (Itr != Json.MemberEnd())                                                               \
    {                                                                                          \
        if (!::L4::Web::Json::Parser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
        {                                                                                      \
            JSON_ERROR_PARSE;                                                                  \
        }                                                                                      \
    }

#define JSON_ITEM_DEF(JsonName, TargetVariable, Default)                                       \
    Itr = Json.FindMember(JSON_STRING(JsonName));                                              \
    if (Itr != Json.MemberEnd())                                                               \
    {                                                                                          \
        if (!::L4::Web::Json::Parser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
        {                                                                                      \
            (TargetVariable) = Default;                                                        \
        }                                                                                      \
    }

#define JSON_ITEM_ROOT(TargetVariable)                                                   \
    bool Parse(const ::L4::Web::Json::Value& Json)                                       \
    {                                                                                    \
        if (!::L4::Web::Json::Parser<decltype(TargetVariable)> {}(Json, TargetVariable)) \
        {                                                                                \
            JSON_ERROR_PARSE;                                                            \
        }                                                                                \
        return true;                                                                     \
    }
