#include "Debug.h"

#include "format/Format.h"

#include <backtrace.h>
#include <client/linux/handler/exception_handler.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

#include <array>
#include <typeinfo>

namespace L4::Debug
{
    backtrace_state* GetBacktraceState() noexcept
    {
        static const auto State = backtrace_create_state(nullptr, 1, nullptr, nullptr);
        return State;
    }

    struct BacktraceEntry
    {
    public:
        BacktraceEntry(uintptr_t ProgramCounter) :
            ProgramCounter(ProgramCounter)
        {
            if (backtrace_pcinfo(GetBacktraceState(), ProgramCounter, &OnBacktraceFull, nullptr, this))
            {
                return;
            }
            if (Function.empty())
            {
                backtrace_syminfo(GetBacktraceState(), ProgramCounter, &OnBacktraceSyminfo, nullptr, this);
            }
        }

        template <class OutItr>
        OutItr Stringize(OutItr Itr) const
        {
            auto Entry = *this;

            Itr = FMT::format_to(Itr, "{:#016x}", Entry.ProgramCounter);

            if (Entry.Symbol.empty())
            {
                Entry.Symbol = "UnknownSymbol";
            }
            if (Entry.Function.empty())
            {
                Entry.Function = "UnknownFunction";
            }
            Itr = FMT::format_to(Itr, " {:s}!{:s}", Entry.Symbol, Entry.Function);

            if (Entry.SymbolValue)
            {
                Itr = FMT::format_to(Itr, "({:#016x})", Entry.SymbolValue);
            }

            if (Entry.File.empty())
            {
                Entry.File = "UnknownFile";

                Itr = FMT::format_to(Itr, " [{:s}]", Entry.File);
            }
            else
            {
                constexpr std::string_view Pattern = "L4\\";
                size_t Offset = Entry.File.rfind(Pattern);
                if (Offset != std::string_view::npos)
                {
                    Entry.File = Entry.File.substr(Offset + Pattern.size());
                }

                Itr = FMT::format_to(Itr, " [{:s}:{:d}]", Entry.File, Entry.LineNumber);
            }

            return Itr;
        }

    private:
        static std::string Demangle(const char* MangledName)
        {
            int Status = 0;
            char* DemangledName = __cxxabiv1::__cxa_demangle(MangledName, nullptr, nullptr, &Status);
            if (!Status)
            {
                std::string Ret;
                Ret = DemangledName;
                __builtin_free(DemangledName);
                return Ret;
            }
            return MangledName;
        }

        static int OnBacktraceFull(void* data, uintptr_t pc, const char* filename, int lineno, const char* function)
        {
            auto BacktraceEntryPtr = reinterpret_cast<BacktraceEntry*>(data);
            if (function)
            {
                BacktraceEntryPtr->Function = Demangle(function);
            }
            if (filename)
            {
                BacktraceEntryPtr->File = filename;
            }
            BacktraceEntryPtr->LineNumber = lineno;
            return function != nullptr;
        }

        static void OnBacktraceSyminfo(void* data, uintptr_t pc, const char* symname, uintptr_t symval, uintptr_t symsize)
        {
            auto BacktraceEntryPtr = reinterpret_cast<BacktraceEntry*>(data);
            if (symname)
            {
                BacktraceEntryPtr->Symbol = Demangle(symname);
            }
            BacktraceEntryPtr->SymbolValue = symval;
        }

        uintptr_t ProgramCounter;
        std::string Function;
        std::string Symbol;
        uintptr_t SymbolValue;
        std::string File;
        int LineNumber;
    };

    size_t CaptureBacktrace(std::span<uintptr_t> Frames)
    {
        struct Impl
        {
            std::span<uintptr_t> Frames;
            size_t FrameCount;
        };

        auto OnBacktraceSimple = +[](void* data, uintptr_t pc) {
            auto DataPtr = reinterpret_cast<Impl*>(data);
            if (DataPtr->FrameCount == DataPtr->Frames.size())
            {
                return 1;
            }
            DataPtr->Frames[DataPtr->FrameCount++] = pc;
            return 0;
        };

        Impl Data { Frames, 0 };
        backtrace_simple(GetBacktraceState(), 0, OnBacktraceSimple, nullptr, &Data);
        return Data.FrameCount;
    }

    std::string GetStackTrace(void* ContextRecord)
    {
        throw std::invalid_argument("Context records do not exist for this platform");
    }

    std::string GetStackTrace(std::span<uintptr_t> Frames)
    {
        std::string Ret;
        Ret.reserve(1024);
        auto OutItr = std::back_inserter(Ret);
        for (uintptr_t FramePtr : Frames)
        {
            OutItr = BacktraceEntry(FramePtr).Stringize(OutItr);
            *OutItr++ = '\n';
        }

        return Ret;
    }

    std::string GetStackTrace()
    {
        std::array<uintptr_t, 128> FramePtr {};
        auto FrameCount = CaptureBacktrace(FramePtr);
        return GetStackTrace(std::span(FramePtr).first(FrameCount));
    }

    bool IsDebuggerPresent()
    {
        return false;
    }

    [[noreturn]] void ExitProcess(int ReturnCode)
    {
        exit(ReturnCode);
    }

    // Defined in LogImpl.lnx.cpp
    std::filesystem::path GetMinidumpDirectory();
    void HandleMinidump(const char* ExistingPath, bool Succeeded);

    static google_breakpad::ExceptionHandler& GetExceptionHandler()
    {
        static constexpr auto Callback = +[](const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) -> bool {
            HandleMinidump(descriptor.path(), succeeded);
            return succeeded;
        };
        static google_breakpad::ExceptionHandler Handler(google_breakpad::MinidumpDescriptor(GetMinidumpDirectory()), nullptr, Callback, nullptr, true, -1);
        return Handler;
    }

    bool WriteMiniDump(const std::filesystem::path& Path, void* ExceptionPointers)
    {
        struct Ctx
        {
            std::reference_wrapper<const std::filesystem::path> Path;
            bool ReturnValue;
        };
        static constexpr auto Callback = +[](const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) -> bool {
            Ctx& Context = *reinterpret_cast<Ctx*>(context);
            std::error_code Error;
            std::filesystem::rename(descriptor.path(), Context.Path, Error);
            Context.ReturnValue = Error && succeeded;
            return succeeded;
        };

        Ctx Context { Path, false };
        GetExceptionHandler().WriteMinidump(Path.parent_path(), Callback, &Context);
        return Context.ReturnValue;
    }

    void SetupExceptions()
    {
        GetExceptionHandler();
    }
}