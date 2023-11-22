#include "mgpch.h"

#include "Application.h"
#include "Services.h"
#include "Timer.h"
#include "Mango/Scene/SceneManager.h"
#include "Mango/Systems/CameraSystem.h"
#include "Mango/Systems/FreePoseSystem.h"
#include "Mango/Systems/ImGuiSystem.h"
#include "Mango/Systems/RenderingSystem.h"
#include "Mango/Systems/SceneGraphSystem.h"
#include "Mango/Window/Input.h"
#include "Mango/Window/Window.h"

#include <cxxopts.hpp>

namespace mango
{
    Application::Application(const ApplicationSettings& appSettings)
        : m_frameTime   (1.0 / appSettings.maxFramerate),
          m_fps         (0),
          m_fpsToReturn (0),
          m_isRunning   (false)
    {
        // Parse command line args.
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

        // Init window
        m_window = std::make_shared<Window>(appSettings.windowWidth, appSettings.windowHeight, appSettings.windowTitle);
    
        if (optResult["fullscreen"].as<bool>())
        {
            m_window->setFullscreen(true);
        }

        // Init Log, Input and GUI systems
        Log::init();
        Input::init(m_window->getNativeWindow());

        // Init core services
        m_eventBus     = new EventBus();
        m_sceneManager = new SceneManager();

        Services::provide(this);
        Services::provide(m_sceneManager);
        Services::provide(m_eventBus);

        // Add core systems - do not forget to update them!
        m_systems.add(new SceneGraphSystem());
        m_systems.configure();

        // TODO: 
        // [x] figure out how to pass Scene* with registry to the systems
        //    so systems could have access to entities
        // [x] System::onUpdate(scene, dt)
        // [x] update systems code
        // [x] renderer system - add EVENTS !!
        // [x] event bus add to services
        // [x] move events to a separate classes (component added etc.)
        // [x] add rendering system include to services.h
        // [x] dtor of Window is not called !!!!
        // [x] dtor of GUI is not called !!!! -> GUI as a system
        // [x] remove system priorities (not needed)
        // [x] Entity: Set position etc. (like in the previous Game Object class)
        // [] camera movement!!

        // Create Rendering system
        // renderingSystem and m_imGuiSystem will be deleted by m_renderingSystems
        RenderingSystem* renderingSystem = new RenderingSystem();
        m_imGuiSystem = new ImGuiSystem();

        m_renderingSystems.add(renderingSystem);
        m_renderingSystems.add(m_imGuiSystem);
        m_renderingSystems.configure();

        Services::provide(renderingSystem);
    }

    Application::~Application()
    {
        delete m_sceneManager;
        delete m_eventBus;
    }

    void Application::addSystem(System* system)
    {
        m_systems.add(system);
        system->onInit();
    }

    unsigned int Application::getFPS() const
    {
        return m_fpsToReturn;
    }

    void Application::start()
    {
        if (m_isRunning)
        {
            return;
        }

        run();
    }

    void Application::stop()
    {
        if (!m_isRunning)
        {
            return;
        }

        m_isRunning = false;
    }

    void Application::run()
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

                if (m_window->isCloseRequested())
                {
                    stop();
                }

                m_systems.updateAll(m_frameTime);
                Input::update();

                if (frameCounter >= 1.0)
                {
                    frames       = 0;
                    frameCounter = 0;
                }
            }

            if (shouldRender)
            {
                m_renderingSystems.updateAll(m_frameTime);

                m_imGuiSystem->being();
                {
                    for (auto& system : m_systems)
                    {
                        system->onGui();
                    }
                }
                m_imGuiSystem->end();

                m_window->endFrame();
                frames++;
            }
        }
    }
}
