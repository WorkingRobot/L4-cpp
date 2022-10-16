#pragma once

#include "Utils.h"

#include <ranges>
#include <span>
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

    static AuthFieldType To(AuthFieldType In)
    {
        return In;
    }

    struct AuthFieldText
    {
        String Placeholder;
    };

    L4_MARSHAL_BETWEEN(AuthFieldText, Placeholder)

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

    static libL4::AuthFieldRadio To(const AuthFieldRadio& In)
    {
        libL4::AuthFieldRadio Ret {
            .EntryCount = uint8_t(In.Entries.size()),
            .DefaultEntryId = To(In.DefaultEntryId)
        };
        for (int i = 0; i < In.Entries.size(); ++i)
        {
            Ret.Entries[i] = To(In.Entries[i]);
        }
        return Ret;
    }

    static AuthFieldRadio To(const libL4::AuthFieldRadio& In)
    {
        AuthFieldRadio Ret {
            .Entries = std::vector<AuthFieldRadioEntry>(In.EntryCount),
            .DefaultEntryId = To(In.DefaultEntryId)
        };
        for (int i = 0; i < In.EntryCount; ++i)
        {
            Ret.Entries[i] = To(In.Entries[i]);
        }
        return Ret;
    }

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
        std::variant<AuthFieldText, AuthFieldRadio, AuthFieldCheckbox, AuthFieldSubmitButton, AuthFieldOpenUrlAction> Data;
    };

    static libL4::AuthField To(const AuthField& In)
    {
        libL4::AuthField Ret {
            .Id = To(In.Id),
            .Name = To(In.Name),
            .Type = In.Type
        };
        switch (In.Type)
        {
        case AuthFieldType::Text:
            Ret.Text = To(std::get<AuthFieldText>(In.Data));
            break;
        case AuthFieldType::Password:
            Ret.Password = To(std::get<AuthFieldPassword>(In.Data));
            break;
        case AuthFieldType::Radio:
            Ret.Radio = To(std::get<AuthFieldRadio>(In.Data));
            break;
        case AuthFieldType::Dropdown:
            Ret.Dropdown = To(std::get<AuthFieldDropdown>(In.Data));
            break;
        case AuthFieldType::Checkbox:
            Ret.Checkbox = To(std::get<AuthFieldCheckbox>(In.Data));
            break;
        case AuthFieldType::SubmitButton:
            Ret.SubmitButton = To(std::get<AuthFieldSubmitButton>(In.Data));
            break;
        case AuthFieldType::OpenUrlAction:
            Ret.OpenUrlAction = To(std::get<AuthFieldOpenUrlAction>(In.Data));
            break;
        }
        return Ret;
    }

    static AuthField To(const libL4::AuthField& In)
    {
        AuthField Ret {
            .Id = To(In.Id),
            .Name = To(In.Name),
            .Type = In.Type
        };
        switch (In.Type)
        {
        case AuthFieldType::Text:
            Ret.Data.emplace<AuthFieldText>(To(In.Text));
            break;
        case AuthFieldType::Password:
            Ret.Data.emplace<AuthFieldPassword>(To(In.Password));
            break;
        case AuthFieldType::Radio:
            Ret.Data.emplace<AuthFieldRadio>(To(In.Radio));
            break;
        case AuthFieldType::Dropdown:
            Ret.Data.emplace<AuthFieldDropdown>(To(In.Dropdown));
            break;
        case AuthFieldType::Checkbox:
            Ret.Data.emplace<AuthFieldCheckbox>(To(In.Checkbox));
            break;
        case AuthFieldType::SubmitButton:
            Ret.Data.emplace<AuthFieldSubmitButton>(To(In.SubmitButton));
            break;
        case AuthFieldType::OpenUrlAction:
            Ret.Data.emplace<AuthFieldOpenUrlAction>(To(In.OpenUrlAction));
            break;
        }
        return Ret;
    }

    struct AuthFulfilledField
    {
        String Id;
        AuthFieldType Type;
        std::variant<String, bool> Data;
    };

    static libL4::AuthFulfilledField To(const AuthFulfilledField& In)
    {
        libL4::AuthFulfilledField Ret {
            .Id = To(In.Id),
            .Type = In.Type
        };
        switch (In.Type)
        {
        case AuthFieldType::Text:
            Ret.Text = To(std::get<String>(In.Data));
            break;
        case AuthFieldType::Password:
            Ret.Password = To(std::get<String>(In.Data));
            break;
        case AuthFieldType::Radio:
            Ret.Radio = To(std::get<String>(In.Data));
            break;
        case AuthFieldType::Dropdown:
            Ret.Dropdown = To(std::get<String>(In.Data));
            break;
        case AuthFieldType::Checkbox:
            Ret.Checkbox = std::get<bool>(In.Data);
            break;
        case AuthFieldType::SubmitButton:
            Ret.SubmitButton = std::get<bool>(In.Data);
            break;
        case AuthFieldType::OpenUrlAction:
            break;
        }
        return Ret;
    }

    static AuthFulfilledField To(const libL4::AuthFulfilledField& In)
    {
        AuthFulfilledField Ret {
            .Id = To(In.Id),
            .Type = In.Type
        };
        switch (In.Type)
        {
        case AuthFieldType::Text:
            Ret.Data.emplace<String>(To(In.Text));
            break;
        case AuthFieldType::Password:
            Ret.Data.emplace<String>(To(In.Password));
            break;
        case AuthFieldType::Radio:
            Ret.Data.emplace<String>(To(In.Radio));
            break;
        case AuthFieldType::Dropdown:
            Ret.Data.emplace<String>(To(In.Dropdown));
            break;
        case AuthFieldType::Checkbox:
            Ret.Data.emplace<bool>(In.Checkbox);
            break;
        case AuthFieldType::SubmitButton:
            Ret.Data.emplace<bool>(In.SubmitButton);
            break;
        case AuthFieldType::OpenUrlAction:
            break;
        }
        return Ret;
    }

    struct AuthSubmitResponse
    {
        bool IsSuccessful;
        String Message;
    };

    L4_MARSHAL_BETWEEN(AuthSubmitResponse, IsSuccessful, Message)
}