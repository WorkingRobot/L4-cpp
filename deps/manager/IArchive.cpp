#include "IArchive.h"

#include <algorithm>

namespace L4::Manager
{
    libL4::Handle IArchive::OpenStreamInternal(uint32_t StreamIdx, bool IsWrite)
    {
        if (std::ranges::find(Streams, StreamIdx, [](const std::unique_ptr<IStream>& Stream) { return Stream->GetIdx(); }) != Streams.end())
        {
            throw std::invalid_argument("Stream is already open");
        }
        
        auto& Ptr = Streams.emplace_back(IsWrite ? OpenStreamWrite(StreamIdx) : OpenStreamRead(StreamIdx));
        return (libL4::Handle)Ptr.get();
    }

    bool IArchive::CloseStreamInternal(libL4::Handle Stream)
    {
        return std::erase_if(Streams, [Stream](const std::unique_ptr<IStream>& StreamPtr) { return (libL4::Handle)StreamPtr.get() == Stream; }) == 1;
    }
}