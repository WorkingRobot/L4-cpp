#pragma once

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace L4::Discord
{
    struct LinearAllocator
    {
        LinearAllocator() = default;

        LinearAllocator(char* buffer, size_t size) :
            buffer_(buffer),
            end_(buffer + size)
        {

        }

        static const bool kNeedFree = false;

        void* Malloc(size_t size)
        {
            char* res = buffer_;
            buffer_ += size;
            if (buffer_ > end_)
            {
                buffer_ = res;
                return nullptr;
            }
            return res;
        }

        void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
        {
            if (newSize == 0)
            {
                return nullptr;
            }
            return Malloc(newSize);
        }

        static void Free(void* ptr)
        {

        }

        char* buffer_;
        char* end_;
    };

    template <size_t Size>
    struct FixedLinearAllocator : public LinearAllocator
    {
        FixedLinearAllocator() :
            LinearAllocator(fixedBuffer_, Size)
        {
        }

        static const bool kNeedFree = false;

        char fixedBuffer_[Size];
    };

    struct DirectStringBuffer
    {
        using Ch = char;
        char* buffer_;
        char* end_;
        char* current_;

        DirectStringBuffer(char* buffer, size_t maxLen) :
            buffer_(buffer), end_(buffer + maxLen), current_(buffer)
        {
        }

        void Put(char c)
        {
            if (current_ < end_)
            {
                *current_++ = c;
            }
        }

        void Flush()
        {
        }

        size_t GetSize() const
        {
            return (size_t)(current_ - buffer_);
        }
    };

    using MallocAllocator = rapidjson::CrtAllocator;
    using PoolAllocator = rapidjson::MemoryPoolAllocator<MallocAllocator>;
    using UTF8 = rapidjson::UTF8<char>;
    // Writer appears to need about 16 bytes per nested object level (with 64bit size_t)
    using StackAllocator = FixedLinearAllocator<2048>;
    constexpr size_t WriterNestingLevels = 2048 / (2 * sizeof(size_t));
    using JsonWriterBase =
        rapidjson::Writer<DirectStringBuffer, UTF8, UTF8, StackAllocator, rapidjson::kWriteNoFlags>;
    class JsonWriter : public JsonWriterBase
    {
    public:
        DirectStringBuffer stringBuffer_;
        StackAllocator stackAlloc_;

        JsonWriter(char* dest, size_t maxLen) :
            JsonWriterBase(stringBuffer_, &stackAlloc_, WriterNestingLevels), stringBuffer_(dest, maxLen), stackAlloc_()
        {
        }

        size_t Size() const
        {
            return stringBuffer_.GetSize();
        }
    };

    using JsonValueBase = rapidjson::GenericValue<UTF8, PoolAllocator>;

    struct JsonValue;

    template<class T>
    struct JsonValueCRTP
    {
        const JsonValue* GetObjectMember(const char* Name) const
        {
            auto Member = static_cast<const T*>(this)->FindMember(Name);
            if (Member != static_cast<const T*>(this)->MemberEnd() && Member->value.IsObject())
            {
                return reinterpret_cast<const JsonValue*>(&Member->value);
            }
            return nullptr;
        }

        int GetIntMember(const char* Name, int Default = 0) const
        {
            auto Member = static_cast<const T*>(this)->FindMember(Name);
            if (Member != static_cast<const T*>(this)->MemberEnd() && Member->value.IsInt())
            {
                return Member->value.GetInt();
            }
            return Default;
        }

        const char* GetStringMember(const char* Name, const char* Default = nullptr) const
        {
            auto Member = static_cast<const T*>(this)->FindMember(Name);
            if (Member != static_cast<const T*>(this)->MemberEnd() && Member->value.IsString())
            {
                return Member->value.GetString();
            }
            return Default;
        }
    };

    struct JsonValue : public JsonValueBase, public JsonValueCRTP<JsonValue>
    {
    };

    using JsonDocumentBase = rapidjson::GenericDocument<UTF8, PoolAllocator, StackAllocator>;
    class JsonDocument : public JsonDocumentBase, public JsonValueCRTP<JsonDocument>
    {
    public:
        static const int kDefaultChunkCapacity = 32 * 1024;
        // json parser will use this buffer first, then allocate more if needed; I seriously doubt we
        // send any messages that would use all of this, though.
        char parseBuffer_[32 * 1024];
        MallocAllocator mallocAllocator_;
        PoolAllocator poolAllocator_;
        StackAllocator stackAllocator_;
        JsonDocument() :
            JsonDocumentBase(rapidjson::kObjectType,
                             &poolAllocator_,
                             sizeof(stackAllocator_.fixedBuffer_),
                             &stackAllocator_),
            poolAllocator_(parseBuffer_, sizeof(parseBuffer_), kDefaultChunkCapacity, &mallocAllocator_), stackAllocator_()
        {
        }
    };
}