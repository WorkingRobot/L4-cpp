#include "StackTrace.h"

// clang-format off
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
// clang-format on

#pragma comment(lib, "DbgHelp.lib")

#include <format>
#include <filesystem>

namespace L4
{
    struct FrameInfo
    {
        DWORD64 FrameAddress;
        std::string_view ModuleName;
        std::string_view SymbolName;
        ULONG64 SymbolAddress;
        DWORD LineNumber;
        std::string_view FileName;
        DWORD64 LineAddress;
    };

    template<class OutItr>
    OutItr StringizeFrameInfo(OutItr Itr, FrameInfo Info)
    {
        Itr = std::format_to(Itr, "{:#016x}", Info.FrameAddress);

        if (Info.ModuleName.empty())
        {
            Info.ModuleName = "UnknownModule";
        }
        else
        {
            size_t Offset = Info.ModuleName.rfind('\\');
            if (Offset != std::string_view::npos)
            {
                Info.ModuleName = Info.ModuleName.substr(Offset + 1);
            }
        }
        if (Info.SymbolName.empty())
        {
            Info.SymbolName = "UnknownFunction";
        }
        Itr = std::format_to(Itr, " {:s}!{:s}", Info.ModuleName, Info.SymbolName);

        if (Info.SymbolAddress)
        {
            Itr = std::format_to(Itr, "({:#016x})", Info.SymbolAddress); 
        }

        if (Info.FileName.empty())
        {
            Info.FileName = "UnknownFile";

            Itr = std::format_to(Itr, " [{:s}]", Info.FileName);
        }
        else
        {
            constexpr std::string_view Pattern = "L4\\src\\";
            size_t Offset = Info.FileName.rfind(Pattern);
            if (Offset != std::string_view::npos)
            {
                Info.FileName = Info.FileName.substr(Offset + Pattern.size());
            }

            Itr = std::format_to(Itr, " [{:s}:{:d}({:#016x})]", Info.FileName, Info.LineNumber, Info.LineAddress);
        }

        return Itr;
    }

    std::string GetStackTrace(CONTEXT* Context)
    {
        auto CurrentProcess = GetCurrentProcess();
        auto CurrentThread = GetCurrentThread();

        SymInitialize(CurrentProcess, NULL, TRUE);

        union
        {
            SYMBOL_INFO Symbol;
            char SymbolBuffer[sizeof(SYMBOL_INFO) + (MAX_SYM_NAME - 1) * sizeof(CHAR)] {};
        };
        Symbol = {
            .SizeOfStruct = sizeof(SYMBOL_INFO),
            .MaxNameLen = MAX_SYM_NAME
        };

        IMAGEHLP_LINE64 Line {
            .SizeOfStruct = sizeof(IMAGEHLP_LINE64)
        };

        STACKFRAME64 StackFrame {
            .AddrPC {
                .Offset = Context->Rip,
                .Mode = AddrModeFlat },
            .AddrFrame {
                .Offset = Context->Rbp,
                .Mode = AddrModeFlat },
            .AddrStack {
                .Offset = Context->Rsp,
                .Mode = AddrModeFlat }
        };

        std::string Ret;
        Ret.reserve(1024);
        auto OutItr = std::back_inserter(Ret);
        while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, CurrentProcess, CurrentThread, &StackFrame, Context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
        {
            FrameInfo Info {
                .FrameAddress = StackFrame.AddrPC.Offset
            };

            char ModuleNameBuffer[MAX_PATH] {};
            HMODULE Module;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)Info.FrameAddress, &Module))
            {
                auto ModuleLength = GetModuleFileNameA(Module, ModuleNameBuffer, MAX_PATH);
                Info.ModuleName = std::string_view(ModuleNameBuffer, ModuleLength);
            }

            if (SymFromAddr(CurrentProcess, Info.FrameAddress, NULL, &Symbol))
            {
                Info.SymbolName = std::string_view(Symbol.Name, Symbol.NameLen);
                Info.SymbolAddress = Symbol.Address;
            }

            DWORD Displacement;
            if (SymGetLineFromAddr64(CurrentProcess, Info.FrameAddress, &Displacement, &Line))
            {
                Info.LineAddress = Line.Address;
                Info.LineNumber = Line.LineNumber;
                Info.FileName = Line.FileName;
            }

            OutItr = StringizeFrameInfo(OutItr, Info);
            *OutItr++ = '\n';
        }

        return Ret;
    }

    std::string GetStackTrace()
    {
        CONTEXT Context {};
        RtlCaptureContext(&Context);
        return GetStackTrace(&Context);
    }
}