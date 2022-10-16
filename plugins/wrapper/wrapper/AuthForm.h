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
            using Type = Wrapper::AuthFieldSubmitButton;
            using FulfilledType = bool;
        };

        template <>
        struct FieldTypeData<Wrapper::AuthFieldType::OpenUrlAction>
        {
            using Type = Wrapper::AuthFieldOpenUrlAction;
            using FulfilledType = void;
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

        const std::vector<Wrapper::AuthField>& Build() const;

    private:
        std::vector<Wrapper::AuthField> Fields;
    };
}