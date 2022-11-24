#include "AuthSession.h"

namespace L4::Plugin::Manager
{
    AuthSession::AuthSession(libL4::PluginInterface Interface) :
        Interface(Interface)
    {
        Interface.Auth.Open(libL4::Handle(this));
    }

    AuthSession::~AuthSession()
    {
        Interface.Auth.Close(libL4::Handle(this));
    }

    const std::vector<libL4::Marshal::AuthField>& AuthSession::GetFields()
    {
        if (ShouldRefreshFields)
        {
            Fields = GetFieldsInternal();
        }
        return Fields;
    }

    libL4::Marshal::AuthSubmitResponse AuthSession::Submit(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields)
    {
        if (Fields.size() > 16)
        {
            throw std::overflow_error("Too many fields (>16)");
        }

        libL4::AuthFulfilledField MarshalledFields[16] {};
        for (int Idx = 0; Idx < Fields.size(); ++Idx)
        {
            MarshalledFields[Idx] = libL4::Marshal::To(Fields[Idx]);
        }
        libL4::AuthSubmitResponse Response;
        Interface.Auth.Submit(libL4::Handle(this), MarshalledFields, Fields.size(), &Response);
        return libL4::Marshal::To(Response);
    }

    std::vector<libL4::Marshal::AuthField> AuthSession::GetFieldsInternal()
    {
        libL4::AuthField Fields[16] {};
        uint32_t FieldCount;
        Interface.Auth.GetFields(libL4::Handle(this), Fields, &FieldCount);

        if (FieldCount > 16)
        {
            throw std::overflow_error("Too many fields (>16)");
        }

        std::vector<libL4::Marshal::AuthField> FieldVec(FieldCount);
        std::ranges::transform(Fields, Fields + FieldCount, FieldVec.begin(), [](const auto& In) { return libL4::Marshal::To(In); });

        return FieldVec;
    }
}