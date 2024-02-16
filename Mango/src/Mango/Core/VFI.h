#pragma once

#include <filesystem>
#include <vector>

namespace mango
{
    class VFI final
    {
    public:
        enum class SearchPathMode : int32_t { PREPEND = 0, APPEND = 1};

    public:
        static void init(const char* arg0);
        static void deinit();

        static void setSaneConfig(const std::string& organization, const std::string& appName);

        static bool addToSearchPath(const std::filesystem::path & path, SearchPathMode mode = SearchPathMode::APPEND);
        static bool removeFromSearchPath(const std::filesystem::path& oldPath);
        static bool setPrefWriteDir(const std::string & organization, const std::string & appName);
        static bool setWriteDir(const std::filesystem::path & dir);

        static std::vector<std::filesystem::path> getSearchPath();
        static std::filesystem::path              getWriteDir();
        static std::filesystem::path              getExecutableDir();

        static std::vector<char> readFile(const std::string& filename);
        static bool              writeFile(const std::string& filename, const void* data, uint64_t sizeBytes, bool append = false);
        static bool              exists(const std::string& filename);

        static std::filesystem::path getFilepath(const std::string& filename);

    private:
        static bool isInitialized;
    };
}