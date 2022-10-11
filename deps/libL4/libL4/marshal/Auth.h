#pragma once

#include "Utils.h"

#include <variant>
#include <vector>

namespace libL4::Marshal
{
    struct UserIdentity
    {
        String Id;
        String Name;
    };

    L4_MARSHAL_BETWEEN(UserIdentity, Id, Name)

    using AuthFieldType = libL4::AuthFieldType;

    struct AuthFieldText
    {
        String Placeholder;
        String VerifyRegex;
        String VerifyErrorMessage;
    };

    L4_MARSHAL_BETWEEN(AuthFieldText, Placeholder, VerifyRegex, VerifyErrorMessage)

    using AuthFieldPassword = AuthFieldText;

    struct AuthFieldRadioEntry
    {
        String Id;
        String Name;
    };

    L4_MARSHAL_BETWEEN(AuthFieldRadioEntry, Id, Name)

    struct AuthFieldRadio
    {
        std::vector<AuthFieldRadioEntry> Entries;
        String DefaultEntryId;
    };

    L4_MARSHAL_BETWEEN(Entries, DefaultEntryId)

    using AuthFieldDropdown = AuthFieldRadio;

    struct AuthFieldCheckbox
    {
        bool IsCheckedByDefault;
    };

    L4_MARSHAL_BETWEEN(AuthFieldCheckbox, IsCheckedByDefault)

    struct AuthFieldSubmitButton
    {
        String Id;
        String Name;
    };

    L4_MARSHAL_BETWEEN(AuthFieldSubmitButton, Id, Name)

    struct AuthFieldOpenUrlAction
    {
        String Url;
    };

    L4_MARSHAL_BETWEEN(AuthFieldOpenUrlAction, Url)

    struct AuthField
    {
        String Id;
        String Name;
        AuthFieldType Type;
        std::variant<AuthFieldText, AuthFieldPassword, AuthFieldRadio, AuthFieldDropdown, AuthFieldCheckbox, AuthFieldOpenUrlAction> Data;
    };

    L4_MARSHAL_BETWEEN(AuthField, Id, Name, Type, Data)

    struct AuthFulfilledField
    {
        String Id;
        AuthFieldType Type;
        std::variant<String, bool> Data;
    };

    L4_MARSHAL_BETWEEN(AuthFulfilledField, Id, Type, Data)
}