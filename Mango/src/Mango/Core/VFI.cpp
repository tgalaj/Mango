#include "mgpch.h"
#include "physfs.h"

namespace mango
{
    bool VFI::isInitialized = false;

    void VFI::init(const char* arg0)
    {
        MG_PROFILE_ZONE_SCOPED;

        isInitialized = PHYSFS_init(arg0);

        MG_CORE_ASSERT_MSG(isInitialized, 
                           "VFI is not initialized!\nPhysFS Error: {}", 
                           PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }

    void VFI::deinit()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        PHYSFS_deinit();
        isInitialized = false;
    }

    void VFI::setSaneConfig(const std::string& organization, const std::string& appName)
    {
        MG_PROFILE_ZONE_SCOPED;
        setPrefWriteDir(organization, appName);

        auto writeDir = getWriteDir();
        auto baseDir = getExecutableDir();

        addToSearchPath(writeDir);
        addToSearchPath(baseDir);
    }

    bool VFI::addToSearchPath(const std::filesystem::path& path, SearchPathMode mode)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        bool ret = PHYSFS_mount(path.string().c_str(), nullptr, int32_t(mode));

        if (!ret)
        {
            MG_CORE_ERROR("Could not add {} to search path!\nPhysFS Error: {}",
                          path.string(),
                          PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        }

        return ret;
    }

    bool VFI::removeFromSearchPath(const std::filesystem::path& oldPath)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        int ret = PHYSFS_removeFromSearchPath(oldPath.string().c_str());

        return (ret != 0) ? true : false;
    }

    bool VFI::setPrefWriteDir(const std::string& organization, const std::string& appName)
    {
        MG_PROFILE_ZONE_SCOPED;
        auto prefDir = PHYSFS_getPrefDir(organization.c_str(), appName.c_str());
        return PHYSFS_setWriteDir(prefDir);
    }

    bool VFI::setWriteDir(const std::filesystem::path& dir)
    {
        MG_PROFILE_ZONE_SCOPED;
        auto writeDir = getWriteDir();

        if (writeDir == dir)
        {
            return true;
        }

        if (!writeDir.empty())
        {
            // Check if writeDir is already in the search path
            auto searchPath = getSearchPath();
            auto it = std::find(searchPath.begin(), searchPath.end(), writeDir);

            // If exists, remove it from the search path
            if (it != searchPath.end())
            {
                PHYSFS_removeFromSearchPath(writeDir.string().c_str());
            }
        }

        return PHYSFS_setWriteDir(dir.string().c_str());
    }

    std::vector<std::filesystem::path> VFI::getSearchPath()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        std::vector<std::filesystem::path> paths;

        for (char** i = PHYSFS_getSearchPath(); *i != NULL; i++)
        {
            paths.emplace_back(*i);
        }

        return paths;
    }

    std::filesystem::path VFI::getWriteDir()
    {
        MG_PROFILE_ZONE_SCOPED;
        auto dir = PHYSFS_getWriteDir();

        if (dir == NULL)
        {
            return std::filesystem::path("");
        }
        else
        {
            return std::filesystem::path(dir);
        }
    }

    std::filesystem::path VFI::getExecutableDir()
    {
        MG_PROFILE_ZONE_SCOPED;
        return std::filesystem::path(PHYSFS_getBaseDir());
    }

    std::vector<char> VFI::readFile(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        PHYSFS_file* file = PHYSFS_openRead(filename.c_str());
        if (file == NULL)
        {
            MG_CORE_ERROR("Could not open {}!\nPhysFS Error: {}",
                          filename,
                          PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
            return {};
        }

        auto fileSize = PHYSFS_fileLength(file);

        std::vector<char> buffer(fileSize);
        auto lengthRead = PHYSFS_readBytes(file, buffer.data(), fileSize);

        PHYSFS_close(file);

        return buffer;
    }

    bool VFI::writeFile(const std::string& filename, const void* data, uint64_t sizeBytes, bool append)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        PHYSFS_file* file = NULL;
        
        if (append)
        {
            file = PHYSFS_openAppend(filename.c_str());
        }
        else
        {
            file = PHYSFS_openWrite(filename.c_str());
        }

        if (file == NULL)
        {
            MG_CORE_ERROR("Could not open {} for writing!\nPhysFS Error: {}",
                          filename,
                          PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
            return false;
        }

        auto lengthWritten = PHYSFS_writeBytes(file, data, sizeBytes);

        if (lengthWritten < sizeBytes)
        {
            MG_CORE_WARN("VFI::writeFile written only {} bytes which is less than requested size ({}).\nPhysFS Error: {}.",
                         lengthWritten,
                         sizeBytes,
                         PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
            return false;
        }

        if (lengthWritten == -1)
        {
            MG_CORE_WARN("Could not write data to {}.\nPhysFS Error: {}.", 
                         filename,
                         PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
            return false;
        }

        PHYSFS_close(file);

        return true;
    }

    bool VFI::exists(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(isInitialized, "VFI is not initialized!");

        return PHYSFS_exists(filename.c_str());
    }

    std::filesystem::path VFI::getFilepath(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        auto realDir = PHYSFS_getRealDir(filename.c_str());

        std::string dir = "";
        if (realDir != NULL)
        {
            dir = std::string(realDir);
        }
        else
        {
            MG_CORE_WARN("Could not find file {}", filename);
        }

        return std::filesystem::path(dir + "/" + filename);
    }
}