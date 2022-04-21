#include "Http.h"

namespace L4::Http
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