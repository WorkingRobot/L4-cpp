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

#define JSON_FORCE_SEMI static_assert(true, "")

// Begin defining a JSON parsing schema
#define JSON_DEFINE \
    void Parse(const ::L4::Web::Json::Value& Json)

// Parse its parent's properties
#define JSON_BASE(BaseClass)    \
    {                           \
        BaseClass::Parse(Json); \
    }                           \
    JSON_FORCE_SEMI

// Parse the given item. Throw if not found or can't be parsed.
#define JSON_ITEM(JsonName, TargetVariable)                                      \
    {                                                                            \
        ::__JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                \
        if (Itr == Json.MemberEnd())                                             \
        {                                                                        \
            throw JSON_ERROR_NOTFOUND;                                           \
        }                                                                        \
        ::__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable); \
    }                                                                            \
    JSON_FORCE_SEMI

// Parse the given item. Ignore if null. Throw if it can't be parsed.
#define JSON_ITEM_NULL(JsonName, TargetVariable)                                     \
    {                                                                                \
        ::__JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                    \
        if (Itr == Json.MemberEnd())                                                 \
        {                                                                            \
            throw JSON_ERROR_NOTFOUND;                                               \
        }                                                                            \
        if (!Itr->value.IsNull())                                                    \
        {                                                                            \
            ::__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable); \
        }                                                                            \
    }                                                                                \
    JSON_FORCE_SEMI

// Parse the given item. Ignore if not found. Throw if it can't be parsed.
#define JSON_ITEM_OPT(JsonName, TargetVariable)                                      \
    {                                                                                \
        ::__JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                    \
        if (Itr != Json.MemberEnd())                                                 \
        {                                                                            \
            ::__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable); \
        }                                                                            \
    }                                                                                \
    JSON_FORCE_SEMI

// Parse the given item. Ignore if not found or can't be parsed. If ignored, set the value to the provided default.
#define JSON_ITEM_DEF(JsonName, TargetVariable, Default)                                 \
    {                                                                                    \
        ::__JsonItr Itr = Json.FindMember(JSON_STRING(JsonName));                        \
        if (Itr != Json.MemberEnd())                                                     \
        {                                                                                \
            try                                                                          \
            {                                                                            \
                ::__JsonParser<decltype(TargetVariable)> {}(Itr->value, TargetVariable); \
            }                                                                            \
            catch (const ::L4::Web::Json::ParseException& Exception)                     \
            {                                                                            \
                (TargetVariable) = Default;                                              \
            }                                                                            \
        }                                                                                \
    }                                                                                    \
    JSON_FORCE_SEMI

// Parse the object itself and place the result in the target variable.
// Useful if parsing an array from a response.
#define JSON_ITEM_ROOT(TargetVariable)                                     \
    {                                                                      \
        ::__JsonParser<decltype(TargetVariable)> {}(Json, TargetVariable); \
    }                                                                      \
    JSON_FORCE_SEMI
