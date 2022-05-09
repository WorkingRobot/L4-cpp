#include "StackTrace.h"

// clang-format off
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
// clang-format on

#pragma comment(lib, "DbgHelp.lib")

#include <array>
#include <format>
#include <span>

namespace L4
{
    struct FrameDataBuffer
    {
        FrameDataBuffer()
        {
            Symbol = {
                .SizeOfStruct = sizeof(SYMBOL_INFO),
                .MaxNameLen = MAX_SYM_NAME
            };
            Line = {
                .SizeOfStruct = sizeof(IMAGEHLP_LINE64)
            };
        }

    private:
        union
        {
            SYMBOL_INFO Symbol;
            char SymbolBuffer[sizeof(SYMBOL_INFO) + (MAX_SYM_NAME - 1) * sizeof(CHAR)] {};
        };
        IMAGEHLP_LINE64 Line;
        char ModuleNameBuffer[MAX_PATH] {};

        friend class FrameInfo;
    };

    struct FrameInfo
    {
        FrameInfo(DWORD64 FrameAddress, FrameDataBuffer& DataBuffer) :
            FrameAddress(FrameAddress)
        {
            HMODULE Module;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)FrameAddress, &Module))
            {
                auto ModuleLength = GetModuleFileNameA(Module, DataBuffer.ModuleNameBuffer, MAX_PATH);
                ModuleName = std::string_view(DataBuffer.ModuleNameBuffer, ModuleLength);
            }
            if (SymFromAddr(GetCurrentProcess(), FrameAddress, NULL, &DataBuffer.Symbol))
            {
                SymbolName = std::string_view(DataBuffer.Symbol.Name, DataBuffer.Symbol.NameLen);
                SymbolAddress = DataBuffer.Symbol.Address;
            }

            DWORD Displacement;
            if (SymGetLineFromAddr64(GetCurrentProcess(), FrameAddress, &Displacement, &DataBuffer.Line))
            {
                LineAddress = DataBuffer.Line.Address;
                LineNumber = DataBuffer.Line.LineNumber;
                FileName = DataBuffer.Line.FileName;
            }
        }

        template <class OutItr>
        OutItr Stringize(OutItr Itr) const
        {
            auto Info = *this;

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

        DWORD64 FrameAddress;
        std::string_view ModuleName;
        std::string_view SymbolName;
        ULONG64 SymbolAddress;
        DWORD LineNumber;
        std::string_view FileName;
        DWORD64 LineAddress;
    };

    std::string GetStackTrace(void* ContextRecord)
    {
        SymInitialize(GetCurrentProcess(), NULL, TRUE);

        FrameDataBuffer DataBuffer;
        STACKFRAME64 StackFrame {};

        std::string Ret;
        Ret.reserve(1024);
        auto OutItr = std::back_inserter(Ret);
        while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(), &StackFrame, ContextRecord, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
        {
            OutItr = FrameInfo(StackFrame.AddrPC.Offset, DataBuffer).Stringize(OutItr);
            *OutItr++ = '\n';
        }

        return Ret;
    }

    std::string GetStackTrace(std::span<PVOID> Frames)
    {
        SymInitialize(GetCurrentProcess(), NULL, TRUE);

        FrameDataBuffer DataBuffer;

        std::string Ret;
        Ret.reserve(1024);
        auto OutItr = std::back_inserter(Ret);
        for (PVOID FramePtr : Frames)
        {
            OutItr = FrameInfo((DWORD64)FramePtr, DataBuffer).Stringize(OutItr);
            *OutItr++ = '\n';
        }

        return Ret;
    }

    std::string GetStackTrace()
    {
        std::array<PVOID, 128> FramePtr {};
        auto FrameCount = RtlCaptureStackBackTrace(0, FramePtr.size(), FramePtr.data(), NULL);
        return GetStackTrace(std::span(FramePtr).first(FrameCount));
    }
}