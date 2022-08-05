#pragma once

#include "ArchiveTree.h"
#include "Base.h"
#include "Guid.h"

namespace L4::Interface
{
    class L4_CLASS_API IStream : public Object<>
    {
    public:
        virtual uint32_t GetId() const = 0;

        virtual Guid GetGuid() const = 0;

        virtual uint32_t GetVersion() const = 0;

        virtual uint32_t GetElementSize() const = 0;

        virtual uint64_t GetName(char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t GetContext(char* Context, uint64_t ContextSize) const = 0;

        virtual uint64_t GetSize() const = 0;

        virtual uint64_t GetCapacity() const = 0;

        virtual uint64_t ReadBytes(uint64_t Offset, char* Dst, uint64_t ByteCount) const = 0;
    };

    class L4_CLASS_API IStreamWritable : public IStream
    {
    public:
        virtual void SetGuid(const Guid& NewGuid) const = 0;

        virtual void SetVersion(uint32_t NewVersion) const = 0;

        virtual void SetElementSize(uint32_t NewElementSize) const = 0;

        virtual uint64_t SetName(const char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t SetContext(const char* Context, uint64_t ContextSize) const = 0;

        virtual uint64_t Reserve(uint64_t NewCapacity) const = 0;

        virtual uint64_t Resize(uint64_t NewSize) const = 0;

        virtual void ShrinkToFit() const = 0;

        virtual uint64_t WriteBytes(uint64_t Offset, const char* Src, uint64_t ByteCount) const = 0;
    };

    class L4_CLASS_API ArchiveFI : public Interface
    {
    public:
        virtual uint32_t GetSectorSize() const = 0;

        virtual uint64_t GetSourceName(char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t GetAppName(char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t GetSourceVersion(char8_t* Version, uint64_t VersionSize) const = 0;

        virtual uint64_t GetAppVersion(char8_t* Version, uint64_t VersionSize) const = 0;

        virtual uint64_t GetEnvironment(char8_t* Environment, uint64_t EnvironmentSize) const = 0;

        virtual uint32_t GetSourceVersionNumeric() const = 0;

        virtual uint32_t GetAppVersionNumeric() const = 0;

        virtual Guid GetSourceGuid() const = 0;

        virtual Guid GetAppGuid() const = 0;

        virtual uint32_t GetStreamCount() const = 0;

        virtual OwningPtr<IStream> OpenStream(uint32_t StreamIdx) const = 0;

        virtual OwningPtr<IStream> OpenStream(const Guid& Guid) const = 0;
    };

    class L4_CLASS_API ArchiveWritableFI : public ArchiveFI
    {
    public:
        virtual uint64_t SetSourceName(const char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t SetAppName(const char8_t* Name, uint64_t NameSize) const = 0;

        virtual uint64_t SetSourceVersion(const char8_t* Version, uint64_t VersionSize) const = 0;

        virtual uint64_t SetAppVersion(const char8_t* Version, uint64_t VersionSize) const = 0;

        virtual uint64_t SetEnvironment(const char8_t* Environment, uint64_t EnvironmentSize) const = 0;

        virtual void SetSourceVersionNumeric(uint32_t NewVersion) const = 0;

        virtual void SetAppVersionNumeric(uint32_t NewVersion) const = 0;

        virtual void SetSourceGuid(const Guid& NewGuid) const = 0;

        virtual void SetAppGuid(const Guid& NewGuid) const = 0;

        virtual OwningPtr<IStreamWritable> OpenStreamWritable(uint32_t StreamIdx) const = 0;

        virtual OwningPtr<IStreamWritable> OpenStreamWritable(const Guid& Guid) const = 0;
    };

    class L4_CLASS_API IArchive : public Object<ArchiveFI>
    {
    protected:
        using Object::Object;

    public:
        virtual Ptr<const ArchiveTree> GetTree() const = 0;

        virtual uint64_t ReadTreeFile(void* Context, uint64_t Offset, char* Data, uint64_t DataSize) const = 0;
    };

    class L4_CLASS_API IArchiveWritable : public IArchive
    {
    protected:
        using IArchive::IArchive;

    };
}