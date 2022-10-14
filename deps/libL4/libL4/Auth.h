#pragma once

#include "Archive.h"

namespace libL4
{
    struct UserIdentity
    {
        String Id;
        String Name;
    };

    enum class AuthFieldType : uint8_t
    {
        Text,
        Password,
        Radio,
        Dropdown,
        Checkbox,
        SubmitButton,
        OpenUrlAction
    };

    struct AuthFieldText
    {
        String Placeholder;
        String VerifyRegex;
        String VerifyErrorMessage;
    };

    typedef AuthFieldText AuthFieldPassword;

    struct AuthFieldRadioEntry
    {
        String Id;
        String Name;
    };

    struct AuthFieldRadio
    {
        AuthFieldRadioEntry Entries[16];
        uint8_t EntryCount;
        String DefaultEntryId;
    };

    typedef AuthFieldRadio AuthFieldDropdown;

    struct AuthFieldCheckbox
    {
        bool IsCheckedByDefault;
    };

    struct AuthFieldSubmitButton
    {
        String Id;
        String Name;
    };

    struct AuthFieldOpenUrlAction
    {
        String Url;
    };

    struct AuthField
    {
        String Id;
        String Name;
        AuthFieldType Type;
        union
        {
            AuthFieldText Text;
            AuthFieldPassword Password;
            AuthFieldRadio Radio;
            AuthFieldDropdown Dropdown;
            AuthFieldCheckbox Checkbox;
            AuthFieldSubmitButton SubmitButton;
            AuthFieldOpenUrlAction OpenUrlAction;
        };
    };

    struct AuthFulfilledField
    {
        String Id;
        AuthFieldType Type;
        union
        {
            String Text;
            String Password;
            String Radio;
            String Dropdown;
            bool Checkbox;
            bool SubmitButton;
        };
    };

    struct AuthOperations
    {
        void (*OnUserUpdated)();
    };

    struct AuthCallbacks
    {
        bool (*GetUser)(UserIdentity* OutIdentity);

        void (*Open)(Handle Auth);

        void (*Close)(Handle Auth);

        void (*GetFields)(Handle Auth, AuthField Fields[16], uint32_t* FieldCount);

        void (*Submit)(Handle Auth, const AuthFulfilledField Fields[16], uint32_t FieldCount);
    };
}