#include "Http.h"

namespace L4::Web::Http
{
    namespace Detail
    {
        cpr::Session CreateSession()
        {
            cpr::Session Session;

            return Session;
        }
    }
}