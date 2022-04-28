#pragma once

#include "StreamDataConstIterator.h"

namespace L4 {
    template <class T = std::byte, class ArchiveT = ArchiveWritable>
    class StreamDataIterator : public StreamDataConstIterator<T, ArchiveT> {
        using Base = StreamDataConstIterator<T, ArchiveT>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        using Base::Base;

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return const_cast<reference>(Base::operator*());
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return const_cast<pointer>(Base::operator->());
        }

        constexpr StreamDataIterator& operator++() noexcept
        {
            Base::operator++();
            return *this;
        }

        constexpr StreamDataIterator operator++(int) noexcept
        {
            StreamDataIterator Tmp = *this;
            Base::operator++();
            return Tmp;
        }

        constexpr StreamDataIterator& operator--() noexcept
        {
            Base::operator--();
            return *this;
        }

        constexpr StreamDataIterator operator--(int) noexcept
        {
            StreamDataIterator Tmp = *this;
            Base::operator--();
            return Tmp;
        }

        constexpr StreamDataIterator& operator+=(const ptrdiff_t Offset) noexcept
        {
            Base::operator+=(Offset);
            return *this;
        }

        [[nodiscard]] constexpr StreamDataIterator operator+(const ptrdiff_t Offset) const noexcept
        {
            StreamDataIterator Tmp = *this;
            Tmp += Offset;
            return Tmp;
        }

        constexpr StreamDataIterator& operator-=(const ptrdiff_t Offset) noexcept
        {
            Base::operator-=(Offset);
            return *this;
        }

        using Base::operator-;

        [[nodiscard]] constexpr StreamDataIterator operator-(const ptrdiff_t Offset) const noexcept
        {
            StreamDataIterator Tmp = *this;
            Tmp -= Offset;
            return Tmp;
        }

        [[nodiscard]] constexpr reference operator[](const ptrdiff_t Offset) const noexcept
        {
            return const_cast<reference>(Base::operator[](Offset));
        }
    };
}
