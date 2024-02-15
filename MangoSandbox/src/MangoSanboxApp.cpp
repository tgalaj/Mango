#include "Mango.h"
#include "Mango/Core/EntryPoint.h"

#include "Sandbox.h"

#include <memory>

class MangoSandboxApp : public mango::Application
{
public:
    MangoSandboxApp(const mango::ApplicationConfiguration& appConfig)
        : Application(appConfig)
    {
        auto rootDir       = std::filesystem::path(MG_ROOT_DIR);
        auto executableDir = mango::VFI::getExecutableDir();

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
    mango::ApplicationConfiguration appConfig{};
                                    appConfig.windowWidth     = 1600;
                                    appConfig.windowHeight    = 800;
                                    appConfig.windowTitle     = "Mango Sandbox";
                                    appConfig.maxFramerate    = 999.0;
                                    appConfig.commandLineArgs = args;

    return new MangoSandboxApp(appConfig);
}
