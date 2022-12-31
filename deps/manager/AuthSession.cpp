#include "AuthSession.h"

#include <algorithm>

namespace L4::Manager
{
    AuthSession::AuthSession(libL4::AuthCallbacks Callbacks) :
        Callbacks(Callbacks)
    {
        Callbacks.Open((libL4::Handle)this);
    }

    AuthSession::~AuthSession()
    {
        Callbacks.Close((libL4::Handle)this);
    }

    std::vector<libL4::Marshal::AuthField> AuthSession::GetFields()
    {
        libL4::AuthField Fields[16] {};
        uint32_t FieldCount = 0;
        Callbacks.GetFields((libL4::Handle)this, Fields, &FieldCount);

        if (FieldCount > 16)
        {
            throw std::overflow_error("Too many fields (>16)");
        }

        std::vector<libL4::Marshal::AuthField> FieldVec(FieldCount);
        std::ranges::transform(Fields, Fields + FieldCount, FieldVec.begin(), [](const auto& In) { return libL4::Marshal::To(In); });

        return FieldVec;
    }

    libL4::Marshal::AuthSubmitResponse AuthSession::Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields)
    {
        if (Fields.size() > 16)
        {
            throw std::invalid_argument("Too many fields (>16)");
        }

        libL4::AuthFulfilledField FulfilledFields[16] {};
        std::ranges::transform(Fields, FulfilledFields, [](const auto& In) { return libL4::Marshal::To(In); });
        libL4::AuthSubmitResponse Response {};
        Callbacks.Submit((libL4::Handle)this, FulfilledFields, Fields.size(), &Response);

        return libL4::Marshal::To(Response);
    }
}