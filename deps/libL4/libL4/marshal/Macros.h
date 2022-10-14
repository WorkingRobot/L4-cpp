#pragma once

#define L4_MARSHAL_EXPAND(x) x

#define L4_MARSHAL_FE_0(WHAT)
#define L4_MARSHAL_FE_1(WHAT, X) L4_MARSHAL_EXPAND(WHAT(X))
#define L4_MARSHAL_FE_2(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_1(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_3(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_2(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_4(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_3(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_5(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_4(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_6(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_5(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_7(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_6(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_8(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_7(WHAT, __VA_ARGS__))
#define L4_MARSHAL_FE_9(WHAT, X, ...) L4_MARSHAL_EXPAND(WHAT(X) L4_MARSHAL_FE_8(WHAT, __VA_ARGS__))
//... repeat as needed

#define L4_MARSHAL_GET_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) NAME
#define L4_MARSHAL_FOR_EACH(action, ...) L4_MARSHAL_EXPAND(L4_MARSHAL_GET_MACRO(_0, __VA_ARGS__, L4_MARSHAL_FE_9, L4_MARSHAL_FE_8, L4_MARSHAL_FE_7, L4_MARSHAL_FE_6, L4_MARSHAL_FE_5, L4_MARSHAL_FE_4, L4_MARSHAL_FE_3, L4_MARSHAL_FE_2, L4_MARSHAL_FE_1, L4_MARSHAL_FE_0)(action, __VA_ARGS__))

#define L4_MARSHAL_TO_MAP(V) .V = To(In.V),
#define L4_MARSHAL_TO(Input, Output, ...)                               \
    static Output To(const Input& In)                                   \
    {                                                                   \
        return { L4_MARSHAL_FOR_EACH(L4_MARSHAL_TO_MAP, __VA_ARGS__) }; \
    }
#define L4_MARSHAL_BETWEEN(Name, ...)             \
    L4_MARSHAL_TO(Name, libL4::Name, __VA_ARGS__) \
    L4_MARSHAL_TO(libL4::Name, Name, __VA_ARGS__)