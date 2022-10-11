#pragma once

#include "Utils.h"

namespace libL4::Marshal
{
    using UpdateState = libL4::UpdateState;

    using UpdatePieceStatus = libL4::UpdatePieceStatus;

    struct UpdateStartInfo
    {
        uint64_t TotalPieceCount;
        uint64_t TotalDownloadByteCount;
        uint64_t TotalReadByteCount;
        uint64_t TotalWriteByteCount;
    };

    L4_MARSHAL_BETWEEN(UpdateStartInfo, TotalPieceCount, TotalDownloadByteCount, TotalReadByteCount, TotalWriteByteCount)

    struct UpdateProgressInfo
    {
        UpdateStartInfo StartInfo;

        uint64_t PieceCount;

        uint64_t DownloadByteCount;
        uint64_t DownloadByteRate;

        uint64_t ReadByteCount;
        uint64_t ReadByteRate;

        uint64_t WriteByteCount;
        uint64_t WriteByteRate;
    };

    L4_MARSHAL_BETWEEN(UpdateProgressInfo, StartInfo, PieceCount, DownloadByteCount, DownloadByteRate, ReadByteCount, ReadByteRate, WriteByteCount, WriteByteRate)
}