#pragma once

#include <source/Archive.h>
#include "Structs.h"

namespace L4::Source::Riot
{
    using namespace L4::Interface;

    template <Guid GuidV, class ValueT, uint32_t VersionV = ValueT::Version>
    class Stream : public Object<IStream>
    {
        template <class ObjT, class... ArgTs>
        friend OwningPtr<ObjT> L4::Interface::CreateObject(ArgTs&&... Args);

    public:
        static constexpr Guid Guid = GuidV;
        using T = ValueT;
        static constexpr uint32_t Version = VersionV;

        class Iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = uint64_t;

            Iterator() noexcept = default;

            Iterator(OwningPtr<const Stream> StreamPtr, uint64_t Position = 0) :
                StreamPtr(StreamPtr), Position(Position)
            {

            }

            [[nodiscard]] value_type operator*() const
            {
                value_type Ret;
                StreamPtr->ReadBytes(Position * sizeof(value_type), (char*)&Ret, sizeof(value_type));
                return Ret;
            }

            Iterator& operator++()
            {
                ++Position;
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator Tmp = *this;
                ++Position;
                return Tmp;
            }

            Iterator& operator--()
            {
                --Position;
                return *this;
            }

            Iterator operator--(int)
            {
                Iterator Tmp = *this;
                --Position;
                return Tmp;
            }

            Iterator& operator+=(const difference_type Offset)
            {
                Position += Offset;
                return *this;
            }

            [[nodiscard]] Iterator operator+(const difference_type Offset) const
            {
                Iterator Tmp = *this;
                Tmp += Offset;
                return Tmp;
            }

            Iterator& operator-=(const difference_type Offset)
            {
                Position -= Offset;
                return *this;
            }

            [[nodiscard]] Iterator operator-(const difference_type Offset) const
            {
                Iterator Tmp = *this;
                Tmp -= Offset;
                return Tmp;
            }

            [[nodiscard]] difference_type operator-(const Iterator& Other) const
            {
                return Position - Other.Position;
            }

            [[nodiscard]] value_type operator[](const difference_type Offset) const
            {
                return *(*this + Offset);
            }

            [[nodiscard]] bool operator==(const Iterator& Other) const
            {
                return Position == Other.Position;
            }

            [[nodiscard]] std::strong_ordering operator<=>(const Iterator& Other) const noexcept
            {
                return Position <=> Other.Position;
            }

        private:
            OwningPtr<const Stream> StreamPtr;
            uint64_t Position;
        };

        template<class SubStreamT>
        class IdIterator : public Iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = typename SubStreamT::T;
            using difference_type = uint64_t;

            IdIterator(OwningPtr<const Stream> IdStreamPtr, OwningPtr<const SubStreamT> SubStreamPtr, uint64_t Position = 0) :
                Iterator(IdStreamPtr, Position), SubStreamPtr(SubStreamPtr)
            {

            }

            [[nodiscard]] value_type operator*() const
            {
                uint64_t Id = Iterator::operator*();
                value_type Ret {};
                SubStreamPtr->ReadBytes(Id * sizeof(value_type), (char*)&Ret, sizeof(value_type));
                return Ret;
            }

            IdIterator& operator++()
            {
                Iterator::operator++();
                return *this;
            }

            IdIterator operator++(int)
            {
                IdIterator Tmp = *this;
                Iterator::operator++();
                return Tmp;
            }

            IdIterator& operator--()
            {
                Iterator::operator--();
                return *this;
            }

            IdIterator operator--(int)
            {
                IdIterator Tmp = *this;
                Iterator::operator--();
                return Tmp;
            }

            IdIterator& operator+=(const difference_type Offset)
            {
                Iterator::operator+=(Offset);
                return *this;
            }

            [[nodiscard]] IdIterator operator+(const difference_type Offset) const
            {
                IdIterator Tmp = *this;
                Tmp += Offset;
                return Tmp;
            }

            IdIterator& operator-=(const difference_type Offset)
            {
                Iterator::operator-=(Offset);
                return *this;
            }

            [[nodiscard]] IdIterator operator-(const difference_type Offset) const
            {
                IdIterator Tmp = *this;
                Tmp -= Offset;
                return Tmp;
            }

            using Iterator::operator-;

            [[nodiscard]] value_type operator[](const difference_type Offset) const
            {
                return *(*this + Offset);
            }

        private:
            OwningPtr<const SubStreamT> SubStreamPtr;
        };

        Stream(FIPtr Interface) :
            Object(Interface)
        {
            if (Interface->GetGuid() != Guid)
            {
                throw Error::StreamGuidMismatch;
            }
            if (Interface->GetVersion() != Version)
            {
                throw Error::StreamVersionMismatch;
            }
            if (Interface->GetElementSize() != sizeof(T))
            {
                throw Error::StreamElementSizeMismatch;
            }
        }

        uint32_t GetId() const
        {
            return Interface->GetId();
        }

        uint64_t SizeBytes() const
        {
            return Interface->GetSize();
        }

        uint64_t Size() const
        {
            return SizeBytes() / sizeof(T);
        }

        uint64_t ReadBytes(uint64_t BytePosition, char* Dst, uint64_t ByteCount) const
        {
            return Interface->ReadBytes(BytePosition, Dst, ByteCount);
        }

        uint64_t Read(uint64_t Position, T* Dst, uint64_t Count) const
        {
            return ReadBytes(Position, (char*)Dst, Count);
        }

        Iterator Begin() const
        {
            return Iterator(OwnedFromThis(this));
        }

        Iterator begin() const
        {
            return Begin();
        }

        Iterator End() const
        {
            return Iterator(OwnedFromThis(this), Size());
        }

        Iterator end() const
        {
            return End();
        }

        template<class T>
        IdIterator<T> IdBegin(OwningPtr<T> SubStreamPtr) const requires(Guid == IdBlobStreamGuid)
        {
            return IdIterator<T>(OwnedFromThis(this), SubStreamPtr);
        }

        template <class T>
        IdIterator<T> IdEnd(OwningPtr<T> SubStreamPtr) const requires(Guid == IdBlobStreamGuid)
        {
            return IdIterator<T>(OwnedFromThis(this), SubStreamPtr, Size());
        }
    };
}