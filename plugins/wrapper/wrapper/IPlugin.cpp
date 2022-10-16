#include "IPlugin.h"

namespace L4::Plugin::Wrapper
{
    IPlugin::IPlugin(const libL4::ClientInterface* ClientInterface, std::unique_ptr<IAuth> AuthInterface, std::unique_ptr<IUpdate> UpdateInterface) :
        Client(ClientInterface),
        Auth(std::move(AuthInterface)),
        Update(std::move(UpdateInterface))
    {
        if (Auth.get() == nullptr)
        {
            throw std::invalid_argument("AuthInterface is null");
        }
        if (Update.get() == nullptr)
        {
            throw std::invalid_argument("UpdateInterface is null");
        }

        if (Instance != nullptr)
        {
            throw std::invalid_argument("Another instance already exists");
        }
        Instance = this;
    }

    libL4::PluginInterface IPlugin::GetRawInterface()
    {
        return {
            .Identity = To(GetIdentity()),
            .Auth = {
                .GetUser = RawOps::Auth::GetUser,
                .Open = RawOps::Auth::Open,
                .Close = RawOps::Auth::Close,
                .GetFields = RawOps::Auth::GetFields,
                .Submit = RawOps::Auth::Submit,
            },
            .Update = {
                .Open = RawOps::Update::Open,
                .Close = RawOps::Update::Close,
                .Start = RawOps::Update::Start,
                .Pause = RawOps::Update::Pause,
                .Resume = RawOps::Update::Resume,
            }
        };
    }

    bool IPlugin::RawOps::Auth::GetUser(libL4::UserIdentity* OutIdentity)
    {
        if (OutIdentity == nullptr)
        {
            throw std::invalid_argument("OutIdentity is null");
        }

        auto Ret = Instance->Auth->GetUser();
        if (Ret.has_value())
        {
            *OutIdentity = To(Ret.value());
            return true;
        }
        return false;
    }

    void IPlugin::RawOps::Auth::Open(libL4::Handle Auth)
    {
        if (!Instance->Auth->CreateSessionInternal(Auth))
        {
            throw std::invalid_argument("Session already exists");
        }
    }

    void IPlugin::RawOps::Auth::Close(libL4::Handle Auth)
    {
        if (!Instance->Auth->CloseSessionInternal(Auth))
        {
            throw std::invalid_argument("Session does not exist");
        }
    }

    void IPlugin::RawOps::Auth::GetFields(libL4::Handle Auth, libL4::AuthField Fields[16], uint32_t* FieldCount)
    {
        if (Fields == nullptr)
        {
            throw std::invalid_argument("Fields is null");
        }
        if (FieldCount == nullptr)
        {
            throw std::invalid_argument("FieldCount is null");
        }

        auto SessionPtr = Instance->Auth->GetSessionInternal(Auth);
        if (SessionPtr == nullptr)
        {
            throw std::invalid_argument("Session does not exist");
        }

        auto FieldVec = SessionPtr->GetFields();
        if (FieldVec.size() > 16)
        {
            throw std::overflow_error("Too many fields (>16)");
        }
        std::ranges::transform(FieldVec, Fields, [](const AuthField& In) { return To(In); });
        *FieldCount = FieldVec.size();
    }

    void IPlugin::RawOps::Auth::Submit(libL4::Handle Auth, const libL4::AuthFulfilledField Fields[16], uint32_t FieldCount, libL4::AuthSubmitResponse* Response)
    {
        if (Fields == nullptr)
        {
            throw std::invalid_argument("Fields is null");
        }
        if (Response == nullptr)
        {
            throw std::invalid_argument("Response is null");
        }

        if (FieldCount > 16)
        {
            throw std::invalid_argument("Too many fields (>16)");
        }

        auto SessionPtr = Instance->Auth->GetSessionInternal(Auth);
        if (SessionPtr == nullptr)
        {
            throw std::invalid_argument("Session does not exist");
        }

        std::vector<AuthFulfilledField> FieldVec(FieldCount);
        std::ranges::transform(Fields, Fields + FieldCount, FieldVec.begin(), [](const auto& In) { return To(In); });

        *Response = To(SessionPtr->Submit(FieldVec));
    }

    void IPlugin::RawOps::Update::Open(libL4::Handle Update, const libL4::AppIdentity* OldIdentity, libL4::AppIdentity* NewIdentity)
    {
        if (OldIdentity == nullptr)
        {
            throw std::invalid_argument("OldIdentity is null");
        }

        if (NewIdentity == nullptr)
        {
            throw std::invalid_argument("NewIdentity is null");
        }

        Wrapper::AppIdentity UpdatedIdentity {};
        if (!Instance->Update->CreateSessionInternal(Update, To(*OldIdentity), UpdatedIdentity))
        {
            throw std::invalid_argument("Session already exists");
        }
        *NewIdentity = To(UpdatedIdentity);
    }

    void IPlugin::RawOps::Update::Close(libL4::Handle Update)
    {
        if (!Instance->Update->CloseSessionInternal(Update))
        {
            throw std::invalid_argument("Session does not exist");
        }
    }

    void IPlugin::RawOps::Update::Start(libL4::Handle Update, libL4::Handle Archive, uint32_t ProgressUpdateRateMs)
    {
        if (ProgressUpdateRateMs < 50)
        {
            throw std::out_of_range("ProgressUpdateRateMs is too small (<50)");
        }
        if (ProgressUpdateRateMs > 10000)
        {
            throw std::out_of_range("ProgressUpdateRateMs is too big (>10000)");
        }

        auto SessionPtr = Instance->Update->GetSessionInternal(Update);
        if (SessionPtr == nullptr)
        {
            throw std::invalid_argument("Session does not exist");
        }

        SessionPtr->Start(Archive, std::chrono::milliseconds(ProgressUpdateRateMs));
    }

    void IPlugin::RawOps::Update::Pause(libL4::Handle Update)
    {
        auto SessionPtr = Instance->Update->GetSessionInternal(Update);
        if (SessionPtr == nullptr)
        {
            throw std::invalid_argument("Session does not exist");
        }

        SessionPtr->Pause();
    }

    void IPlugin::RawOps::Update::Resume(libL4::Handle Update)
    {
        auto SessionPtr = Instance->Update->GetSessionInternal(Update);
        if (SessionPtr == nullptr)
        {
            throw std::invalid_argument("Session does not exist");
        }

        SessionPtr->Resume();
    }
}