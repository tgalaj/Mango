#include "mgpch.h"

#include "Application.h"
#include "CoreServices.h"
#include "Mango/GUI/GUI.h"
#include "Mango/Systems/CameraSystem.h"
#include "Mango/Systems/ConsoleSystem.h"
#include "Mango/Systems/FreePoseSystem.h"
#include "Mango/Systems/GUISystem.h"
#include "Mango/Systems/RenderingSystem.h"
#include "Mango/Systems/SceneGraphSystem.h"
#include "Mango/Utilities/Timer.h"
#include "Mango/Window/Input.h"
#include "Mango/Window/Window.h"

#include <cxxopts.hpp>

mango::Application::Application(const ApplicationSettings& appSettings)
    : m_usersSystems(entities, events),
      m_frameTime   (1.0 / appSettings.maxFramerate),
      m_fps         (0),
      m_fpsToReturn (0),
      m_isRunning   (false)
{
    /* Parse command line args. */
    cxxopts::Options options(appSettings.windowTitle, "");

    options.add_options()
        ("w,width", "Window width", cxxopts::value<uint32_t>())
        ("h,height", "Window height", cxxopts::value<uint32_t>())
        ("fullscreen", "Set fullscreen window", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));

    auto optResult = options.parse(appSettings.commandLineArgs.argsCount, appSettings.commandLineArgs.argsValues);

    if (optResult.count("width"))
    {
        const_cast<ApplicationSettings&>(appSettings).windowWidth = optResult["width"].as<uint32_t>();
    }

    if (optResult.count("height"))
    {
        const_cast<ApplicationSettings&>(appSettings).windowHeight = optResult["height"].as<uint32_t>();
    }

    /* Init window */
    Window::create(appSettings.windowWidth, appSettings.windowHeight, appSettings.windowTitle);
    
    if (optResult["fullscreen"].as<bool>())
    {
        Window::setFullscreen(true);
    }

    /* Init Log, Input and GUI systems */
    Log::init();
    Input::init(Window::getNativeWindow());
    GUI::init(Window::getNativeWindow());

    /* Add core systems - do not forget to update them! */
    //systems.add<AudioSystem>();
    systems.add<SceneGraphSystem>();
    systems.add<ConsoleSystem>();
    systems.add<CameraSystem>();
    systems.add<GUISystem>();
    systems.add<RenderingSystem>();
    systems.add<FreePoseSystem>();
    systems.configure();

    /* Configure user's systems */
    m_usersSystems.configure();

    /* Set up Core Services */
    CoreServices::provide(this);
    CoreServices::provide(systems.system<RenderingSystem>().get());
}

mango::Application::~Application()
{

}

void mango::Application::setGame(const std::shared_ptr<BaseGame>& game)
{
    m_game = game;
    m_game->init();
}

unsigned int mango::Application::getFPS() const
{
    return m_fpsToReturn;
}

void mango::Application::start()
{
    if (m_isRunning)
    {
        return;
    }

    run();
}

void mango::Application::stop()
{
    if (!m_isRunning)
    {
        return;
    }

    m_isRunning = false;
}

void mango::Application::updateSystems(entityx::TimeDelta dt)
{
    /**
     * Update core engine systems in specific order
     */
    //systems.update<AudioSystem>();
    systems.update<ConsoleSystem>(dt);
    systems.update<FreePoseSystem>(dt);
    systems.update<CameraSystem>(dt);
    systems.update<SceneGraphSystem>(dt);

    m_usersSystems.update_all(dt);
}

void mango::Application::updateRenderingSystems(entityx::TimeDelta dt)
{
    systems.update<RenderingSystem>(dt);
    systems.update<GUISystem>(dt);
}

void mango::Application::run()
{
    m_isRunning = true;

    int    frames       = 0;
    double frameCounter = 0.0;

    double lastTime        = Timer::getTime();
    double unprocessedTime = 0.0;

    double startTime  = 0.0;
    double passedTime = 0.0;

    bool shouldRender = false;

    while (m_isRunning)
    {
        shouldRender = false;

        startTime  = Timer::getTime();
        passedTime = startTime - lastTime;

        lastTime = startTime;

        unprocessedTime += passedTime;
        frameCounter    += passedTime;

        while (unprocessedTime > m_frameTime)
        {
            shouldRender = true;

            unprocessedTime -= m_frameTime;

            if (Window::isCloseRequested())
            {
                stop();
            }

            m_game->input(float(m_frameTime));
            updateSystems(m_frameTime);
            Input::update();

            if (frameCounter >= 1.0)
            {
                frames       = 0;
                frameCounter = 0;
            }
        }

        if (shouldRender)
        {
            /* Update Rendering and GUI systems */
            updateRenderingSystems(m_frameTime);

            Window::endFrame();
            frames++;
        }
    }
}
