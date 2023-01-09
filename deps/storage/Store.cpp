#include "Store.h"

#include "streams/FileStream.h"

namespace L4::Storage
{
    class Store::Impl
    {
    public:
        Impl(const std::filesystem::path& Path) :
            File(Path, FileStream::OpenMode::ReadWrite, FileStream::CreateMode::CreateOrOpen)
        {
            if (File.Size() != 0)
            {
                File >> Map;
            }
        }

        ~Impl()
        {
            File.Truncate();
            File << Map;
        }

        void Set(const std::string_view Key, std::vector<std::byte>&& Data)
        {
            Map.emplace(Key, std::move(Data));
        }

        const std::vector<std::byte>& Get(const std::string_view Key)
        {
            return Map.try_emplace(std::string(Key)).first->second;
        }

    private:
        std::unordered_map<std::string, std::vector<std::byte>> Map;
        FileStream File;        
    };

    Store::Store(const std::filesystem::path& Path) :
        PImpl(std::make_unique<Impl>(Path))
    {
    }

    Store::~Store() = default;

    void Store::SetRaw(const std::string_view Key, std::vector<std::byte>&& Data)
    {
        PImpl->Set(Key, std::move(Data));
    }

    const std::vector<std::byte>& Store::GetRaw(const std::string_view Key) const
    {
        return PImpl->Get(Key);
    }
}