#pragma once

#include "format/Format.h"

#include <concepts>
#include <source_location>

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

        [[nodiscard]] static consteval std::string_view FixFilename(const std::string_view Filename)
        {
            constexpr std::string_view Pattern = "L4\\";
            size_t Offset = Filename.rfind(Pattern);
            if (Offset != Filename.npos)
            {
                return Filename.substr(Offset + Pattern.size());
            }
            return Filename;
        }

        struct SourceLocation
        {
            [[nodiscard]] consteval SourceLocation(const std::source_location Location) :
                Line(Location.line()),
                Column(Location.column()),
                File(FixFilename(Location.file_name())),
                Function(Location.function_name())
            {
            }

            uint_least32_t Line;
            uint_least32_t Column;
            std::string_view File;
            std::string_view Function;
        };

        static constexpr auto MessageFormat = FMT_COMPILE("{:s}: {:s} ({:s} @ {:s}({:d},{:d}))\n");

        template <LogLevel Level, class StoreT>
        [[nodiscard]] static inline std::string FormatDynamicMessage(const auto Message, const SourceLocation Location, StoreT Store)
        {
            return fmt::format(MessageFormat, LogLevelToString(Level), fmt::vformat(Message, Store), Location.Function, Location.File, Location.Line, Location.Column);
        }

        template <LogLevel Level>
        [[nodiscard]] static inline std::string FormatMessage(const std::string_view Message, const SourceLocation Location)
        {
            return fmt::format(MessageFormat, LogLevelToString(Level), Message, Location.Function, Location.File, Location.Line, Location.Column);
        }

        template <class ArgsT>
        struct FormatStringFor
        {
        };

        template <class ContextT, class... ArgTs>
#if fmt == fmt
        struct FormatStringFor<fmt::format_arg_store<ContextT, ArgTs...>>
        {
            using Type = fmt::format_string<std::type_identity_t<ArgTs>...>;
            static constexpr size_t ArgCount = sizeof...(ArgTs);
        };
#elif defined(CONFIG_VERSION_PLATFORM_win)
        struct FormatStringFor<std::_Format_arg_store<ContextT, ArgTs...>>
        {
            using Type = std::format_string<std::type_identity_t<ArgTs>...>;
            static constexpr size_t ArgCount = sizeof...(ArgTs);
        };
#else
#error Unknown formatting library implementation
#endif

        template <LogLevel Level, class StoreT>
        struct MessageCtx
        {
        private:
            using FormatStringFor = FormatStringFor<StoreT>;
            using FormatStringForT = std::conditional_t<FormatStringFor::ArgCount == 0, std::string_view, typename FormatStringFor::Type>;

        public:
#ifdef __INTELLISENSE__
            // std::source_location::current() implodes intellisense for whatever reason, says "internal error", and refuses to function
            consteval MessageCtx(std::convertible_to<FormatStringForT> auto Message, const std::source_location Location = {})
#else
            consteval MessageCtx(std::convertible_to<FormatStringForT> auto Message, const std::source_location Location = std::source_location::current()) :
#endif
                Message(Message),
                Location(Location)
            {
            }

            [[nodiscard]] inline std::string Format(const StoreT& Store) const
            {
                if constexpr (FormatStringFor::ArgCount == 0)
                {
                    return FormatMessage<Level>(Message, Location);
                }
                else
                {
                    return FormatDynamicMessage<Level>(Message, Location, Store);
                }
            }

            const FormatStringForT Message;
            const SourceLocation Location;
        };

        static constexpr auto EmptyArgs = fmt::make_format_args();
        static constexpr auto EmptyEvaluator = []() -> const decltype(EmptyArgs)& { return EmptyArgs; };

        void Abort(const std::string& Message);

        void LogRaw(LogLevel Level, const std::string& Message);
    }

    void SetupLogging();

    template <LogLevel Level, class StoreT>
    inline void Log(Detail::MessageCtx<Level, std::type_identity_t<StoreT>> Message, StoreT Store)
    {
        if constexpr (Level != LogLevel::Critical)
        {
            Detail::LogRaw(Level, Message.Format(Store));
        }
        else
        {
            Detail::Abort(Message.Format(Store));
        }
    }

    template <LogLevel Level, class FuncT>
    inline void Ensure(bool Condition, Detail::MessageCtx<Level, std::invoke_result_t<FuncT>> Message, FuncT Evaluator = Detail::EmptyEvaluator)
    {
        if (!Condition) [[unlikely]]
        {
            Log<Level>(Message, Evaluator());
        }
    }
}

#define LOG(Level, Message, ...) (::L4::Log<LogLevel::Level>((Message), fmt::make_format_args(__VA_ARGS__)))

#define ENSURE(Level, Condition, Message, ...) (::L4::Ensure<LogLevel::Level>((Condition), (Message), []() { return fmt::make_format_args(__VA_ARGS__); }))

#define ABORT(Message, ...) (::L4::Log<LogLevel::Critical>((Message), fmt::make_format_args(__VA_ARGS__)))

#define ASSUME(Condition, Message, ...) (::L4::Ensure<LogLevel::Critical>((Condition), (Message), []() { return fmt::make_format_args(__VA_ARGS__); }))
