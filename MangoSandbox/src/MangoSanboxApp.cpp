#include "Mango.h"
#include "Mango/Core/EntryPoint.h"

#include "Sandbox.h"

#include <memory>

class MangoSandboxApp : public mango::Application
{
public:
    MangoSandboxApp(const mango::ApplicationSettings& appSettings)
        : Application(appSettings)
    {
        auto rootDir = std::filesystem::path(MG_ROOT_DIR);
        auto executableDir = mango::VFI::getExecutableDir();

        // TODO: Add below search paths to config
        mango::VFI::setWriteDir(executableDir / "output");

        mango::VFI::addToSearchPath(executableDir);
        mango::VFI::addToSearchPath(rootDir / "MangoTestAssets");
        mango::VFI::addToSearchPath(rootDir / "MangoSandbox/assets");

        MG_TRACE("Search path:");
        for (auto const& p : mango::VFI::getSearchPath())
        {
            MG_TRACE("  - {}", p.string());
        }

        addRuntimeSystem(new Sandbox());
    }

    ~MangoSandboxApp()
    {

    }
};

mango::Application* mango::createApplication(mango::ApplicationCommandLineArgs args)
{
    mango::ApplicationSettings appSettings{};
                               appSettings.windowWidth     = 1600;
                               appSettings.windowHeight    = 800;
                               appSettings.windowTitle     = "Mango Sandbox";
                               appSettings.maxFramerate    = 999.0;
                               appSettings.commandLineArgs = args;

    return new MangoSandboxApp(appSettings);
}
