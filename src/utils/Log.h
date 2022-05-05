#pragma once

#include <cstdint>
#include <format>
#include <concepts>

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

        static consteval std::string_view LogLevelToEscColorCode(LogLevel Level)
        {
            switch (Level)
            {
            case LogLevel::Critical:
                return "\33[31m"; // Dark Red
            case LogLevel::Error:
                return "\33[31;1m"; // Bold/Bright Red
            case LogLevel::Warning:
                return "\33[33;1m"; // Bold/Bright Yellow
            case LogLevel::Info:
                return "\33[37;1m"; // Bold/Bright White
            case LogLevel::Debug:
                return "\33[36;1m"; // Bold/Bright Aqua
            default:
                return "\33[32;1m"; // Bold/Bright Green
            }
        }

        static constexpr std::string_view ResetEscColorCode = "\33[0m";

        static constexpr std::string_view FixFilename(const std::string_view Filename)
        {
            constexpr std::string_view Pattern = "src\\";
            size_t Offset = Filename.rfind(Pattern);
            std::string_view FixedFilename = Offset == std::string_view::npos ? Filename : Filename.substr(Offset + Pattern.size());
            return FixedFilename;
        }
        
        // Until this is fixed: https://godbolt.org/z/415z5jna9, this is a workaround
        struct SourceLocation
        {
            [[nodiscard]]
            static constexpr SourceLocation Current(
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

        template<LogLevel Level>
        static consteval auto CreateFormatter()
        {
            return [=](std::string_view Message, const SourceLocation Location, const std::format_args Args) {
                return std::format("{}: {} ({} @ {})", LogLevelToString(Level), std::vformat(Message, Args), Location.File, Location.Line);
            };
        }

        template <LogLevel Level>
        static consteval auto CreateFormatterNoArgs()
        {
            return [=](std::string_view Message, const SourceLocation Location) {
                return std::format("{}: {} ({} @ {})", LogLevelToString(Level), Message, Location.File, Location.Line);
            };
        }

        template <LogLevel Level, class... ArgTs>
        static auto FormatMessage(std::string_view Message, const SourceLocation Location, ArgTs&&... Args)
        {
            if constexpr (sizeof...(ArgTs) == 0)
            {
                return CreateFormatterNoArgs<Level>()(Message, Location);
            }
            return CreateFormatter<Level>()(Message, Location, std::make_format_args(std::forward<ArgTs>(Args)...));
        }

        template <class... ArgTs>
        struct MessageCtx
        {
            template <class T>
            requires std::convertible_to<const T&, std::string_view>
            consteval MessageCtx(const T& Message, const SourceLocation Location = SourceLocation::Current()) :
                Message(std::_Fmt_string<ArgTs...>(Message)._Str),
                Location(Location)
            {

            }

            const std::string_view Message;
            const Detail::SourceLocation Location;
        };
    }



    template <LogLevel Level, class... ArgTs>
    void Log(Detail::MessageCtx<std::type_identity_t<ArgTs>...> Message, ArgTs&&... Args)
    {
        printf("%s\n", Detail::FormatMessage<Level>(Message.Message, Message.Location, std::forward<ArgTs>(Args)...).c_str());
    }
}