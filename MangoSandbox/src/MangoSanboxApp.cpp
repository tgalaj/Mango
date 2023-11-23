#include "Mango.h"
#include "Mango/Core/EntryPoint.h"

#include "TestDemo.h"

#include <memory>

class TestApp : public mango::Application
{
public:
    TestApp(const mango::ApplicationSettings& appSettings)
        : Application(appSettings)
    {
        addSystem(new TestDemo());
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

    return testApp;
}
