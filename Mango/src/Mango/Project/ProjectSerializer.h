#include "Project.h"

#include <filesystem>

#define MG_PROJECT_EXTENSION ".mgproj"

namespace mango
{
    class ProjectSerializer
    {
    public:
        ProjectSerializer()  = delete;
        ~ProjectSerializer() = delete;

        static bool         serialize(const ref<Project>& project, const std::filesystem::path& outFilepath);
        static ref<Project> deserialize(const std::filesystem::path& inFilepath);
    };
}