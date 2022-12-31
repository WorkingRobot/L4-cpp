#pragma once

#include "format/Format.h"

#include <concepts>
#include <cstdint>
#include <string_view>

namespace L4
{
    enum class LogLevel : uint8_t
    {
        Critical,
        Error,
        Warning,
        Info,
        Debug
    };

    namespace Detail
    {
        static consteval std::string_view LogLevelToString(LogLevel Level)
        {
            switch (Level)
            {
            case LogLevel::Critical:
                return "Critical";
            case LogLevel::Error:
                return "Error";
            case LogLevel::Warning:
                return "Warning";
            case LogLevel::Info:
                return "Info";
            case LogLevel::Debug:
                return "Debug";
            default:
                return "Unknown";
            }
        }

        static constexpr std::string_view FixFilename(const std::string_view Filename)
        {
            constexpr std::string_view Pattern = "src\\";
            size_t Offset = Filename.rfind(Pattern);
            std::string_view FixedFilename = Offset == std::string_view::npos ? Filename : Filename.substr(Offset + Pattern.size());
            return FixedFilename;
        }

        // Until this is fixed in MSVC: https://godbolt.org/z/415z5jna9, this is a workaround
        // Update: Fixed in v19.29 VS16.10
        struct SourceLocation
        {
            [[nodiscard]] static constexpr SourceLocation Current(
                const uint_least32_t Line = __builtin_LINE(), const uint_least32_t Column = __builtin_COLUMN(),
                const char* const File = __builtin_FILE(), const char* const Function = __builtin_FUNCTION()) noexcept
            {
                return {
                    .Line = Line,
                    .Column = Column,
                    .File = FixFilename(File),
                    .Function = Function
                };
            }

            uint_least32_t Line {};
            uint_least32_t Column {};
            std::string_view File = "";
            std::string_view Function = "";
        };

        template <size_t ArgCount, class FuncT>
        static auto inline FormatMessage(std::string_view Message, FuncT Evaluator)
        {
            if constexpr (ArgCount == 0)
            {
                return Message;
            }
            else
            {
                return FMT::vformat(Message, Evaluator());
            }
        }

        template <LogLevel Level, size_t ArgCount, class FuncT>
        static auto inline FormatMessage(std::string_view Message, const SourceLocation Location, FuncT Evaluator)
        {
            return FMT::format("{}: {} ({}({},{}) @ {})\n", LogLevelToString(Level), FormatMessage<ArgCount>(Message, Evaluator), Location.File, Location.Line, Location.Column, Location.Function);
        }

        template <class ArgsT>
        struct FormatStringFor
        {
        };

        template <class ContextT, class... ArgTs>
#if 1
        struct FormatStringFor<fmt::format_arg_store<ContextT, ArgTs...>>
        {
            using Type = fmt::format_string<std::type_identity_t<ArgTs>...>;
            static constexpr size_t ArgCount = sizeof...(ArgTs);
        };
#else
        struct FormatStringFor<std::_Format_arg_store<ContextT, ArgTs...>>
        {
            using Type = std::format_string<std::type_identity_t<ArgTs>...>;
            static constexpr size_t ArgCount = sizeof...(ArgTs);
        };
#endif

        template <class ArgsT>
        using FormatStringForT = typename FormatStringFor<ArgsT>::Type;

        static constexpr auto EmptyEvaluator = []() { return FMT::make_format_args(); };

        template <class ArgsT>
        struct MessageCtx
        {
        private:
            // Provide a compile time error if the format string is invalid
            struct FormattedMessage
            {
                template <class T>
                consteval FormattedMessage(const T& Message) :
                    Message(Message),
                    Formatted(Message)
                {
                }

                const std::string_view Message;
                const FormatStringForT<ArgsT> Formatted;
            };

        public:
            template <class T>
            requires std::convertible_to<const T&, std::string_view>
            consteval MessageCtx(const T& Message, const SourceLocation Location = SourceLocation::Current()) :
                Message(FormattedMessage(Message).Message),
                Location(Location)
            {
            }

            template <LogLevel Level, class FuncT>
            requires(std::is_same_v<ArgsT, std::invoke_result_t<FuncT>>)
            [[nodiscard]] inline std::string Format(FuncT Evaluator) const
            {
                return FormatMessage<Level, FormatStringFor<ArgsT>::ArgCount>(Message, Location, Evaluator);
            }

            const std::string_view Message;
            const Detail::SourceLocation Location;
        };

        void Abort(const std::string& Message);

        void LogRaw(LogLevel Level, const std::string& Message);
    }

    void SetupLogging();

    template <LogLevel Level, class FuncT = decltype(Detail::EmptyEvaluator)>
    inline void Log(Detail::MessageCtx<std::invoke_result_t<FuncT>> Message, FuncT Evaluator = Detail::EmptyEvaluator)
    {
        if constexpr (Level != LogLevel::Critical)
        {
            Detail::LogRaw(Level, Message.template Format<Level>(Evaluator));
        }
        else
        {
            Detail::Abort(Message.template Format<Level>(Evaluator));
        }
    }

    template <LogLevel Level, class FuncT = decltype(Detail::EmptyEvaluator)>
    inline void Ensure(bool Condition, Detail::MessageCtx<std::invoke_result_t<FuncT>> Message, FuncT Evaluator = Detail::EmptyEvaluator)
    {
        if (!Condition)
            [[unlikely]]
        {
            Log<Level>(Message, Evaluator);
        }
    }

    template <class FuncT = decltype(Detail::EmptyEvaluator)>
    inline void Abort(Detail::MessageCtx<std::invoke_result_t<FuncT>> Message, FuncT Evaluator = Detail::EmptyEvaluator)
    {
        Log<LogLevel::Critical>(Message, Evaluator);
    }

    template <LogLevel Level, class FuncT = decltype(Detail::EmptyEvaluator)>
    inline void Verify(bool Condition, Detail::MessageCtx<std::invoke_result_t<FuncT>> Message, FuncT Evaluator = Detail::EmptyEvaluator)
    {
        if (!Condition)
            [[unlikely]]
        {
            Log<LogLevel::Critical>(Message, Evaluator);
        }
    }
}
