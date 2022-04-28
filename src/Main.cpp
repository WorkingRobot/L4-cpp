#include "archive/ArchiveCreator.h"
#include "archive/StreamDataIterator.h"
#include "archive/StreamView.h"
#include "BasicDisk.h"
#include "RamDisk.h"

#include <conio.h>
#include <string.h>
//#include "web/Http.h"

#include <numeric>

static constexpr bool PrintAlloc = false;
void* operator new(std::size_t sz) // no inline, required by [replacement.functions]/3
{
    if constexpr (PrintAlloc) {
        std::printf("global op new called, size = %zu\n", sz);
    }
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success

    if (void* ptr = std::malloc(sz))
        return ptr;

    throw std::bad_alloc {}; // required by [new.delete.single]/3
}
void operator delete(void* ptr) noexcept
{
    if constexpr (PrintAlloc) {
        std::puts("global op delete called");
    }
    std::free(ptr);
}

namespace L4 {
    void Main()
    {
        ExFatTime Now = std::chrono::zoned_time { std::chrono::time_point_cast<centiseconds>(std::chrono::system_clock::now()) };
        IntervalList FileList;
        FileList.Add(0, 1, std::span("122939293949939323949229", 24));
        ExFatDirectory Directory {
            .Directories = {
                CreateDirectory(L"Test", { CreateDirectory(L"A", {}, { CreateFile(L"B") }) }, { CreateFile(L"C") }) },
            .Files = { CreateFile(L"D", FileList, 24) }
        };
        Disk::BasicDisk Disk(Directory);
        Disk.Start();
        _getch();
        // FastSpd::VirtualDisk(0, 0);
        return;
        // auto Resp = Http::Get(cpr::Url{ "https://httpbin.org/json" }, Http::ReserveSize{4532});
        // printf("Resp capacity %zu\n", Resp.text.capacity());
        // return;

        /*
        {
            MmioFileWritable File("ar.iar");
            std::byte NoCtx[192]{};
            CreateArchive(File,
                SectorSize{ 1 << 16 },
                SourceInfo{ u8"L4", u8"v0.1", 1, {0, 0, 0, 0} },
                AppInfo{ u8"Test", u8"", 0, {~0u, ~0u, ~0u, ~0u} },
                StreamInfo{
                    { {1, 1, 1, 1}, 0, sizeof(int), u8"Stream A", NoCtx}
                }
            );

            ArchiveWritable Archive(std::move(File));
            StreamViewWritable Stream(Archive, 0);
            Stream.Resize(1ll << 25);

            StreamDataIterator<int> Itr(Archive, 0);
            std::iota(Itr, Itr + (1ll << 25), 1);
        }
        {
            Archive Archive("ar.iar");
            StreamDataConstIterator<int> Itr(Archive, 0);
            for (int i = 0; i < 1 << 25; ++i)
            {
                if (*Itr != i + 1)
                {
                    __debugbreak();
                }
                ++Itr;
            }
            __debugbreak();
        }*/
    }
}

int main(int argc, char* argv[])
{
    L4::Main();

    return 0;
}