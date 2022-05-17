#pragma once

#include <stdint.h>

template<class T>
concept ReadableStream = requires(T& Stream, char* Dst, size_t Count)
{
    { Stream.Read(Dst, Count) } -> std::same_as<T&>;
};

template<class T>
concept WritableStream = requires(T& Stream, const char* Src, size_t Count)
{
    { Stream.Write(Src, Count) } -> std::same_as<T&>;
};

template<class T>
concept SeekableStream = requires(T& Stream, size_t Position)
{
    { Stream.SeekBegin(Position) } -> std::same_as<T&>;
    { Stream.SeekAhead(Position) } -> std::same_as<T&>;
    { Stream.SeekEnd(Position) } -> std::same_as<T&>;
};

template<class T>
concept TellableStream = requires(const T& Stream)
{
    { Stream.Tell() } -> std::same_as<size_t>;
};

template<class T>
concept SizableStream = requires(const T& Stream)
{
    { Stream.Size() } -> std::same_as<size_t>;
};

template <ReadableStream StreamT, class T>
StreamT& operator>>(StreamT& Stream, T& Val) requires(std::is_trivially_copyable_v<T>)
{
    return Stream.Read((char*)&Val, sizeof(Val));
}

template <WritableStream StreamT, class T>
StreamT& operator<<(StreamT& Stream, const T& Val) requires(std::is_trivially_copyable_v<T>)
{
    return Stream.Write((const char*)&Val, sizeof(Val));
}