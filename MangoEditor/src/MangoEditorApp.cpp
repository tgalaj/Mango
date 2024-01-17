#include "Mango.h"
#include "Mango/Core/EntryPoint.h"

#include "EditorSystem.h"

#include <memory>

class MangoEditorApp : public mango::Application
{
public:
    MangoEditorApp(const mango::ApplicationSettings& appSettings)
        : Application(appSettings)
    {
        auto rootDir = std::filesystem::path(MG_ROOT_DIR);
        auto executableDir = mango::VFI::getExecutableDir();

        mango::VFI::setWriteDir(executableDir / "output");

        mango::VFI::addToSearchPath(executableDir);
        mango::VFI::addToSearchPath(rootDir / "MangoTestAssets");
        mango::VFI::addToSearchPath(rootDir / "MangoSandbox/assets");
        mango::VFI::addToSearchPath(rootDir / "MangoEditor/assets");

        MG_TRACE("Search path:");
        for (auto const& p : mango::VFI::getSearchPath())
        {
            MG_TRACE("  - {}", p.string());
        }

        addSystem(new EditorSystem());
    }

    ~MangoEditorApp()
    {

    }
};

mango::Application* mango::createApplication(mango::ApplicationCommandLineArgs args)
{
    mango::ApplicationSettings appSettings{};
                               appSettings.windowWidth     = 1920;
                               appSettings.windowHeight    = 1080;
                               appSettings.windowTitle     = "Test App";
                               appSettings.maxFramerate    = 999.0;
                               appSettings.commandLineArgs = args;

    return new MangoEditorApp(appSettings);
}
