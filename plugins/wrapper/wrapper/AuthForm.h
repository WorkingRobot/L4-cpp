#pragma once

#include <libL4/marshal/Marshal.h>
#include <libL4/Structs.h>

#include <optional>

namespace L4::Plugin::Wrapper
{
    using namespace libL4::Marshal;

    namespace Detail
    {
        template <Wrapper::AuthFieldType Type>
        struct FieldTypeData;

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::Label>
        {
            using Type = std::monostate;
            using FulfilledType = std::monostate;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::Text>
        {
            using Type = Wrapper::AuthFieldText;
            using FulfilledType = Wrapper::String;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::Password>
        {
            using Type = Wrapper::AuthFieldPassword;
            using FulfilledType = Wrapper::String;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::Radio>
        {
            using Type = Wrapper::AuthFieldRadio;
            using FulfilledType = Wrapper::String;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::Dropdown>
        {
            using Type = Wrapper::AuthFieldDropdown;
            using FulfilledType = Wrapper::String;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::Checkbox>
        {
            using Type = Wrapper::AuthFieldCheckbox;
            using FulfilledType = bool;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::SubmitButton>
        {
            using Type = std::monostate;
            using FulfilledType = bool;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::OpenUrlAction>
        {
            using Type = Wrapper::AuthFieldOpenUrlAction;
            using FulfilledType = std::monostate;
        };

        template <Wrapper::AuthFieldType Type>
        using FieldT = typename FieldTypeData<Type>::Type;

        template <Wrapper::AuthFieldType Type>
        using FulfilledFieldT = typename FieldTypeData<Type>::FulfilledType;
    }

    class AuthForm
    {
    public:
        AuthForm() = default;

        template <Wrapper::AuthFieldType Type, class... ArgTs>
        void Add(const std::u8string& Id, const std::u8string& Name, ArgTs&&... Args)
        {
            Fields.emplace_back(Wrapper::AuthField {
                .Id = Id,
                .Name = Name,
                .Type = Type,
                .Data = Detail::FieldT<Type> { std::forward<ArgTs>(Args)... },
            });
        }

        const std::vector<Wrapper::AuthField>& Build() const
        {
            return Fields;
        }

    private:
        std::vector<Wrapper::AuthField> Fields;
    };

    class AuthFulfilledForm
    {
    public:
        AuthFulfilledForm(const std::vector<Wrapper::AuthFulfilledField>& Fields) :
            Fields(Fields)
        {
        }

        template <Wrapper::AuthFieldType Type>
        const auto& Get(const std::u8string& Id) const
        {
            auto FieldItr = std::ranges::find(Fields, Id, &Wrapper::AuthFulfilledField::Id);
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
        std::vector<Wrapper::AuthFulfilledField> Fields;
    };
}