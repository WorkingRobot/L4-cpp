#include "utils/Log.h"
#include "archive/ArchiveCreator.h"
#include "archive/StreamDataIterator.h"
#include "archive/StreamView.h"
#include <L4/web/Http.h>

namespace L4
{
    void Main()
    {
        LogSetup();
        {
            MmioFileWritable File("ar.iar");
            std::byte NoCtx[184] {};
            CreateArchive(File,
                          SectorSize { 1 << 16 },
                          SourceInfo { u8"L4", u8"v0.1", 1, { 0, 0, 0, 0 } },
                          AppInfo { u8"Test", u8"", 0, { ~0u, ~0u, ~0u, ~0u } },
                          StreamInfo {
                              { { 1, 1, 1, 1 }, 0, sizeof(int), u8"Stream A", NoCtx } });

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
        }
    }
}

int main(int argc, char* argv[])
{
    L4::Main();
    return 0;
}