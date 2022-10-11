#pragma once

#include "SourceStructs.h"

#include <memory>

namespace L4::SourceWrapper
{
    struct Version
    {
        std::u8string Humanized;
        uint32_t Numeric;
    };

    struct BaseIdentity
    {
        std::u8string Id;
        std::u8string Name;
    };

    struct VersionedIdentity
    {
        std::u8string Id;
        std::u8string Name;
        Version Version;
    };

    using L4Identity = VersionedIdentity;

    struct UserIdentity
    {
        std::u8string Id;
        std::u8string DisplayName;
    };

    struct AuthMethod
    {
        std::u8string Id;
        std::u8string DisplayName;
        std::u8string Icon;
    };

    struct AuthMethodFieldText
    {
        std::u8string Placeholder;
        std::u8string VerifyRegex;
        std::u8string VerifyErrorMessage;
        bool IsPassword;
    };

    struct AuthMethodFieldCheckbox
    {
        bool IsCheckedByDefault;
    };

    struct AuthMethodFieldMultiEntry
    {
        std::u8string Id;
        std::u8string Name;
    };

    struct AuthMethodFieldMulti
    {
        std::vector<AuthMethodFieldMultiEntry> Entries;
        std::u8string DefaultEntryId;
        bool IsRadio;
    };

    struct AuthMethodFieldSubmit
    {
    };

    struct AuthMethodField
    {
        std::u8string Id;
        std::u8string Name;
        std::variant<AuthMethodFieldText, AuthMethodFieldCheckbox, AuthMethodFieldMulti, AuthMethodFieldSubmit> Data;
    };

    class IAuthFlow
    {
    public:
        IAuthFlow();

        virtual ~IAuthFlow() = default;

        virtual const std::vector<AuthMethodField>& GetFields() const;
    };

    class IInterface
    {
    public:
        IInterface(const L4Identity& Identity);

        virtual ~IInterface() = default;

        virtual VersionedIdentity GetIdentity() const = 0;

        virtual UserIdentity GetUserIdentity() = 0;

        virtual std::unique_ptr<IAuthFlow> CreateAuthFlow(const AuthMethod& Method) = 0;

        virtual std::unique_ptr<IUpdate> CreateUpdate(const Source::AppIdentity& OldIdentity, Source::AppIdentity& NewIdentity) = 0;
    };

    class IUpdate
    {
    public:
        IUpdate(const Source::AppIdentity& OldIdentity, Source::AppIdentity& NewIdentity);

        virtual ~IUpdate() = default;

        virtual void Start(Source::Archive Archive, std::chrono::milliseconds ProgressUpdateRate) = 0;

        virtual void Pause() = 0;

        virtual void Resume() = 0;

    protected:
        Source::UpdateState GetState();

        void OnStart(const Source::UpdateStartInfo&);

        void OnProgress(const Source::UpdateProgressInfo&);

        void OnPieceUpdate(uint64_t, Source::UpdatePieceStatus);

        void OnFinalize();

        void OnComplete();

    private:
    };

    // Forgive me, for I have created a factory...
    struct Factory
    {
        static std::unique_ptr<IInterface> CreateInterface(const L4Identity& Identity);
    };
}