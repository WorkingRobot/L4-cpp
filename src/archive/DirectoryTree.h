#pragma once

#include "ArchiveView.h"
#include "Entry.h"

#include <iterator>

namespace L4
{
    template<size_t ChunkSize>
    class DirectoryTreeChunk
    {
    public:
        using value_type = EntryBase;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;

        class const_iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;

            using value_type = DirectoryTreeChunk<ChunkSize>::value_type;
            using difference_type = DirectoryTreeChunk<ChunkSize>::difference_type;
            using pointer = DirectoryTreeChunk<ChunkSize>::const_pointer;
            using reference = const value_type&;

            const_iterator() noexcept : Chunk(), Idx()
            {
            }

            const_iterator(DirectoryTreeChunk<ChunkSize>* Chunk, uint32_t Idx) noexcept : Chunk(Chunk), Idx(Idx)
            {
            }

            [[nodiscard]] reference operator*() const noexcept
            {
                return *(pointer)&Chunk->Data[Idx];
            }

            [[nodiscard]] pointer operator->() const noexcept
            {
                return (pointer)&Chunk->Data[Idx];
            }

            const_iterator& operator++() noexcept
            {
                Idx += GetEntrySize((*this)->Size);
                return *this;
            }

            const_iterator operator++(int) noexcept
            {
                const_iterator Tmp = *this;
                operator++();
                return Tmp;
            }

            [[nodiscard]] bool operator==(const const_iterator& Other) const noexcept
            {
                return Chunk == Other.Chunk && (Idx == Other.Idx || (_AtEnd() && Other._AtEnd()));
            }

            [[nodiscard]] bool operator==(std::default_sentinel_t) const noexcept
            {
                return Chunk == nullptr;
            }

        protected:
            [[nodiscard]] bool _AtChunkEnd() const noexcept
            {
                return Idx >= ChunkSize;
            }

            [[nodiscard]] bool _AtEnd() const noexcept
            {
                return _AtChunkEnd() || (*this)->Magic != EntryMagic;
            }

            DirectoryTreeChunk<ChunkSize>* Chunk;
            uint32_t Idx;
        };
        static_assert(std::forward_iterator<const_iterator>, "Not a forward iterator");

        class iterator : public const_iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;

            using value_type = DirectoryTreeChunk<ChunkSize>::value_type;
            using difference_type = DirectoryTreeChunk<ChunkSize>::difference_type;
            using pointer = DirectoryTreeChunk<ChunkSize>::pointer;
            using reference = value_type&;

            using const_iterator::const_iterator;

            [[nodiscard]] reference operator*() const noexcept
            {
                return const_cast<reference>(const_iterator::operator*());
            }

            [[nodiscard]] pointer operator->() const noexcept
            {
                return const_cast<pointer>(const_iterator::operator->());
            }

            iterator& operator++() noexcept
            {
                const_iterator::operator++();
                return *this;
            }

            iterator operator++(int) noexcept
            {
                iterator Tmp = *this;
                const_iterator::operator++();
                return Tmp;
            }

            template<class TBase, class T = std::remove_reference_t<TBase>, class Traits = EntryTraits<T>>
            void set(TBase&& Value)
            {
                if (const_iterator::_AtChunkEnd())
                {
                    throw std::out_of_range{ "end of chunk" };
                }
                if (const_iterator::_AtEnd() || VerifyEntry<T, Traits>(**this))
                {
                    *(T*)&**this = std::forward<T>(Value);
                }
            }
        };
        static_assert(std::forward_iterator<iterator>, "Not a forward iterator");

        iterator begin() noexcept
        {
            return iterator(this, 0);
        }

        const_iterator begin() const noexcept
        {
            return const_iterator(this, 0);
        }

        const_iterator cbegin() const noexcept
        {
            return const_iterator(this, 0);
        }

        iterator end() noexcept
        {
            return iterator(this, ChunkSize);
        }

        const_iterator end() const noexcept
        {
            return const_iterator(this, ChunkSize);
        }

        const_iterator cend() const noexcept
        {
            return const_iterator(this, ChunkSize);
        }

    private:
        char Data[ChunkSize];
    };

    template<size_t ChunkSize>
    class DirectoryTreeView : ArchiveView
    {
        static_assert(ChunkSize == ClusterSize || ChunkSize == SectorSize);
        using Chunk = DirectoryTreeChunk<ChunkSize>;
        using ContinuatorEntry = std::conditional_t<ChunkSize == ClusterSize, EntryNonResidentCluster, EntryNonResidentSector>;

    public:
        DirectoryTreeView() noexcept : Head() {}

        DirectoryTreeView(ArchiveView View, uint32_t HeadIdx) noexcept :
            ArchiveView(View),
            Head(ChunkSize == ClusterSize ?
                GetClusterPtr<DirectoryTreeChunk<ChunkSize>>(HeadIdx) :
                GetSectorPtr<DirectoryTreeChunk<ChunkSize>>(HeadIdx)
            )
        {}

        DirectoryTreeView(ArchiveView View, DirectoryTreeChunk<ChunkSize>* Head) noexcept : ArchiveView(View), Head(Head) {}

        class const_iterator : public DirectoryTreeChunk<ChunkSize>::const_iterator, protected ArchiveView
        {
            static uint32_t GetIndex(const ContinuatorEntry& Entry)
            {
                if constexpr (ChunkSize == ClusterSize)
                {
                    return Entry.ClusterIdx;
                }
                else
                {
                    return Entry.SectorIdx;
                }
            }
            Chunk* GetChunkPtr(uint32_t ChunkIdx)
            {
                if constexpr (ChunkSize == ClusterSize)
                {
                    return GetClusterPtr<Chunk>(ChunkIdx);
                }
                else
                {
                    return GetSectorPtr<Chunk>(ChunkIdx);
                }
            }

        public:
            using base = DirectoryTreeChunk<ChunkSize>::const_iterator;
            using iterator_category = std::forward_iterator_tag;

            using value_type = base::value_type;
            using difference_type = base::difference_type;
            using pointer = base::pointer;
            using reference = base::reference;

            const_iterator() noexcept : base(), ArchiveView() {}

            const_iterator(ArchiveView View, DirectoryTreeChunk<ChunkSize>* Chunk, uint32_t Idx) noexcept : base(Chunk, Idx), ArchiveView(View) {}

            const_iterator& operator++() noexcept
            {
                base::operator++();
                if (base::_AtEnd())
                {
                    auto Idx = GetIndex(GetEntry<ContinuatorEntry>(*this->Chunk->begin()));
                    if (Idx != InvalidIdx)
                    {
                        this->Chunk = GetChunkPtr(Idx);
                        Idx = 0;
                    }
                }
                return *this;
            }

            const_iterator operator++(int) noexcept
            {
                const_iterator Tmp = *this;
                operator++();
                return Tmp;
            }
        };
        static_assert(std::forward_iterator<const_iterator>, "Not a forward iterator");

        class iterator : public const_iterator
        {
        public:
            using base = DirectoryTreeChunk<ChunkSize>::iterator;
            using iterator_category = std::forward_iterator_tag;

            using value_type = base::value_type;
            using difference_type = base::difference_type;
            using pointer = base::pointer;
            using reference = base::reference;

            using const_iterator::const_iterator;

            [[nodiscard]] reference operator*() const noexcept
            {
                return const_cast<reference>(const_iterator::operator*());
            }

            [[nodiscard]] pointer operator->() const noexcept
            {
                return const_cast<pointer>(const_iterator::operator->());
            }

            iterator& operator++() noexcept
            {
                const_iterator::operator++();
                return *this;
            }

            iterator operator++(int) noexcept
            {
                iterator Tmp = *this;
                const_iterator::operator++();
                return Tmp;
            }

            template<class TBase, class T = std::remove_reference_t<TBase>, class Traits = EntryTraits<T>>
            void set(TBase&& Value)
            {
                if (const_iterator::_AtChunkEnd())
                {
                    throw std::out_of_range{ "allocate next chunk" };
                }
                if (const_iterator::_AtEnd() || VerifyEntry<T>(**this))
                {
                    *(T*)&**this = std::forward<T>(Value);
                }
            }
        };
        static_assert(std::forward_iterator<iterator>, "Not a forward iterator");

        iterator begin() noexcept
        {
            return iterator(*this, Head, 0);
        }

        const_iterator begin() const noexcept
        {
            return const_iterator(*this, Head, 0);
        }

        const_iterator cbegin() const noexcept
        {
            return const_iterator(*this, Head, 0);
        }

        iterator end() noexcept
        {
            return iterator(*this, Head, ChunkSize);
        }

        const_iterator end() const noexcept
        {
            return const_iterator(*this, Head, ChunkSize);
        }

        const_iterator cend() const noexcept
        {
            return const_iterator(*this, Head, ChunkSize);
        }

    private:
        DirectoryTreeChunk<ChunkSize>* Head;
    };

    using DirectoryTreeClusterChunk = DirectoryTreeChunk<ClusterSize>;
    using DirectoryTreeClusterView = DirectoryTreeView<ClusterSize>;

    using DirectoryTreeSectorChunk = DirectoryTreeChunk<SectorSize>;
    using DirectoryTreeSectorView = DirectoryTreeView<SectorSize>;
}