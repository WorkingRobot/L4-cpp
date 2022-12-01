#pragma once

#include "Json.h"

#ifndef JSON_ERROR_PARSE
#define JSON_ERROR_PARSE
#endif

#ifndef JSON_ERROR_NOTFOUND
#define JSON_ERROR_NOTFOUND
#endif

#ifndef JSON_STRING_PREFIX
#define JSON_STRING_PREFIX u8
#endif

#define JSON_STRING2(a, b) a##b
#define JSON_STRING1(a, b) JSON_STRING2(a, b)
#define JSON_STRING(str) JSON_STRING1(JSON_STRING_PREFIX, str)

#define JSON_STRING_TYPE ::std::JSON_STRING(string)

using JsonString = JSON_STRING_TYPE;
using __JsonItr = ::L4::Web::Json::Value::ConstMemberIterator;
template <typename T>
using __JsonParser = ::L4::Web::Json::Parser<T>;

class __JsonReturn
{
    bool Value;

public:
    __JsonReturn() :
        Value(true)
    {
    }

    __JsonReturn(bool Value) :
        Value(Value)
    {
    }

    operator bool() const noexcept
    {
        return Value;
    }
};

#define JSON_FORCE_SEMI static_assert(true, "")

#define JSON_DEFINE \
    __JsonReturn Parse(const ::L4::Web::Json::Value& Json)

#define JSON_BASE(BaseClass)         \
    {                                \
        if (!BaseClass::Parse(Json)) \
        {                            \
            JSON_ERROR_PARSE;        \
            return false;            \
        }                            \
    }                                \
    JSON_FORCE_SEMI

#define JSON_ITEM(JsonName, TargetVariable)                                         \
    {                                                                               \
        __JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                     \
        if (Itr == Json.MemberEnd())                                                \
        {                                                                           \
            JSON_ERROR_NOTFOUND;                                                    \
            return false;                                                           \
        }                                                                           \
        if (!__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
        {                                                                           \
            JSON_ERROR_PARSE;                                                       \
            return false;                                                           \
        }                                                                           \
    }                                                                               \
    JSON_FORCE_SEMI

#define JSON_ITEM_NULL(JsonName, TargetVariable)                                                            \
    {                                                                                                       \
        __JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                                             \
        if (Itr == Json.MemberEnd())                                                                        \
        {                                                                                                   \
            JSON_ERROR_NOTFOUND;                                                                            \
            return false;                                                                                   \
        }                                                                                                   \
        if (!Itr->value.IsNull() && !__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
        {                                                                                                   \
            JSON_ERROR_PARSE;                                                                               \
            return false;                                                                                   \
        }                                                                                                   \
    }                                                                                                       \
    JSON_FORCE_SEMI

#define JSON_ITEM_OPT(JsonName, TargetVariable)                                         \
    {                                                                                   \
        __JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                         \
        if (Itr != Json.MemberEnd())                                                    \
        {                                                                               \
            if (!__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
            {                                                                           \
                JSON_ERROR_PARSE;                                                       \
                return false;                                                           \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
    JSON_FORCE_SEMI

#define JSON_ITEM_DEF(JsonName, TargetVariable, Default)                                \
    {                                                                                   \
        __JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                         \
        if (Itr != Json.MemberEnd())                                                    \
        {                                                                               \
            if (!__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable)) \
            {                                                                           \
                (TargetVariable) = Default;                                             \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
    JSON_FORCE_SEMI

#define JSON_ITEM_ROOT(TargetVariable)                                        \
    bool Parse(const ::L4::Web::Json::Value& Json)                            \
    {                                                                         \
        if (!__JsonParser<decltype(TargetVariable)> {}(Json, TargetVariable)) \
        {                                                                     \
            JSON_ERROR_PARSE;                                                 \
            return false;                                                     \
        }                                                                     \
        return true;                                                          \
    }
