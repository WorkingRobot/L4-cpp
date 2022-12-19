#pragma once

#include "Response.h"

namespace L4::Web::Http
{
    class Client
    {
    protected:
        template <class T>
        using Response = Response<T>;
    };
}