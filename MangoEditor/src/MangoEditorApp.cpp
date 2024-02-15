#include "Mango.h"
#include "Mango/Core/EntryPoint.h"

#include "EditorSystem.h"

namespace mango
{
    class MangoEditorApp : public Application
    {
    public:
        MangoEditorApp(const ApplicationConfiguration& appConfig)
            : Application(appConfig)
        {
            auto rootDir = std::filesystem::path(MG_ROOT_DIR);
            VFI::addToSearchPath(rootDir / "MangoEditor/resources");

            addEditorSystem(new EditorSystem());
        }

        ~MangoEditorApp()
        {

        }
    };

    Application* createApplication(ApplicationCommandLineArgs args)
    {
        ApplicationConfiguration appConfig{};
        appConfig.windowWidth     = 1920;
        appConfig.windowHeight    = 1080;
        appConfig.windowTitle     = "Mango Editor";
        appConfig.maxFramerate    = 999.0;
        appConfig.commandLineArgs = args;
        appConfig.maximized       = false;

        return new MangoEditorApp(appConfig);
    }
}
