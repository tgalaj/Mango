#include "Mango.h"
#include "Mango/Core/EntryPoint.h"

#include "EditorSystem.h"

#include <memory>

namespace mango
{
    class MangoEditorApp : public Application
    {
    public:
        MangoEditorApp(const ApplicationSettings& appSettings)
            : Application(appSettings)
        {
            auto rootDir       = std::filesystem::path(MG_ROOT_DIR);
            auto executableDir = VFI::getExecutableDir();

            // TODO: Add below search paths to config
            VFI::setWriteDir(executableDir / "output");

            VFI::addToSearchPath(executableDir);
            VFI::addToSearchPath(rootDir / "MangoTestAssets");
            VFI::addToSearchPath(rootDir / "MangoSandbox/assets");
            VFI::addToSearchPath(rootDir / "MangoEditor/assets");

            MG_TRACE("Search path:");
            for (auto const& p : VFI::getSearchPath())
            {
                MG_TRACE("  - {}", p.string());
            }

            addSystem(new EditorSystem());
        }

        ~MangoEditorApp()
        {

        }
    };

    Application* createApplication(ApplicationCommandLineArgs args)
    {
        ApplicationSettings appSettings{};
        appSettings.windowWidth     = 1920;
        appSettings.windowHeight    = 1080;
        appSettings.windowTitle     = "Mango Editor";
        appSettings.maxFramerate    = 999.0;
        appSettings.commandLineArgs = args;

        return new MangoEditorApp(appSettings);
    }
}
