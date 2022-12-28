#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    namespace Detail
    {
        template <libL4::AuthFieldType Type>
        struct FieldTypeData;

        template <>
        struct FieldTypeData<libL4::AuthFieldType::Label>
        {
            using Type = std::monostate;
            using FulfilledType = std::monostate;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::Text>
        {
            using Type = libL4::Marshal::AuthFieldText;
            using FulfilledType = libL4::Marshal::String;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::Password>
        {
            using Type = libL4::Marshal::AuthFieldPassword;
            using FulfilledType = libL4::Marshal::String;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::Radio>
        {
            using Type = libL4::Marshal::AuthFieldRadio;
            using FulfilledType = libL4::Marshal::String;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::Dropdown>
        {
            using Type = libL4::Marshal::AuthFieldDropdown;
            using FulfilledType = libL4::Marshal::String;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::Checkbox>
        {
            using Type = libL4::Marshal::AuthFieldCheckbox;
            using FulfilledType = bool;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::SubmitButton>
        {
            using Type = std::monostate;
            using FulfilledType = bool;
        };

        template <>
        struct FieldTypeData<libL4::AuthFieldType::OpenUrlAction>
        {
            using Type = libL4::Marshal::AuthFieldOpenUrlAction;
            using FulfilledType = std::monostate;
        };

        template <libL4::AuthFieldType Type>
        using FieldT = typename FieldTypeData<Type>::Type;

        template <libL4::AuthFieldType Type>
        using FulfilledFieldT = typename FieldTypeData<Type>::FulfilledType;
    }

    class AuthForm
    {
    public:
        AuthForm() = default;

        template <libL4::AuthFieldType Type, class... ArgTs>
        void Add(const std::u8string& Id, const std::u8string& Name, ArgTs&&... Args)
        {
            Fields.emplace_back(libL4::Marshal::AuthField {
                .Id = Id,
                .Name = Name,
                .Type = Type,
                .Data = Detail::FieldT<Type> { std::forward<ArgTs>(Args)... },
            });
        }

        const std::vector<libL4::Marshal::AuthField>& Build() const
        {
            return Fields;
        }

    private:
        std::vector<libL4::Marshal::AuthField> Fields;
    };

    class AuthFulfilledForm
    {
    public:
        AuthFulfilledForm(const std::vector<libL4::Marshal::AuthFulfilledField>& Fields) :
            Fields(Fields)
        {
        }

        template <libL4::AuthFieldType Type>
        const auto& Get(const std::u8string& Id) const
        {
            auto FieldItr = std::ranges::find(Fields, Id, &libL4::Marshal::AuthFulfilledField::Id);
            if (FieldItr == Fields.end())
            {
                throw std::invalid_argument("No such id exists");
            }
            const auto& Field = *FieldItr;
            if (Field.Type != Type)
            {
                throw std::invalid_argument("Field has invalid type");
            }
            return std::get<Detail::FulfilledFieldT<Type>>(Field.Data);
        }

    private:
        std::vector<libL4::Marshal::AuthFulfilledField> Fields;
    };
}