#include "Project.h"

#include <filesystem>

#define MG_PROJECT_EXTENSION ".mgproj"

namespace mango
{
    class ProjectSerializer
    {
    public:
        ProjectSerializer() = delete;
        ~ProjectSerializer() = delete;

        static bool                     serialize(const std::shared_ptr<Project>& project, const std::filesystem::path& outFilepath);
        static std::shared_ptr<Project> deserialize(const std::filesystem::path& inFilepath);
    };
}