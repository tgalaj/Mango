#include <Mango.h>

#include "Scenes/TestDemo.h"
#include "Systems/MoveSystem.h"

#include <memory>

class TestApp : public mango::Application
{
public:
    TestApp(const mango::ApplicationSettings& appSettings)
        : Application(appSettings)
    {
        addSystem<MoveSystem>();
    }

    ~TestApp()
    {

    }
};

mango::Application* mango::createApplication(mango::ApplicationCommandLineArgs args)
{
    mango::ApplicationSettings appSettings{};
                               appSettings.windowWidth     = 1600;
                               appSettings.windowHeight    = 800;
                               appSettings.windowTitle     = "Test App";
                               appSettings.maxFramerate    = 999.0;
                               appSettings.commandLineArgs = args;

    auto testApp = new TestApp(appSettings);
    testApp->setGame(std::make_shared<TestDemo>());

    return testApp;
}
