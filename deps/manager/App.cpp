#include "App.h"

#include <stdexcept>

namespace L4::Manager
{
    App::App(const std::filesystem::path& Path)
    {
        throw std::invalid_argument("Unimplemeneted");
    }

    App::App(const libL4::Marshal::ArchiveIdentity& Identity, const std::filesystem::path& Path, uint32_t SectorSize, uint32_t MaxStreamCount)
    {
        throw std::invalid_argument("Unimplemented");
    }

    const libL4::Marshal::ArchiveIdentity& App::GetIdentity() const noexcept
    {
        return Identity;
    }
}