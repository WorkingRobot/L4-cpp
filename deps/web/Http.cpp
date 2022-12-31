#include "Http.h"

namespace L4::Web::Http
{
    namespace Detail
    {
        void InitializeSession(cpr::Session& Session)
        {
            if constexpr (true)
            {
                Session.SetProxies({
                    { "http", "localhost:8888" },
                    { "https", "localhost:8888" },
                });
                Session.SetVerifySsl(false);
            }
        }
    }
}