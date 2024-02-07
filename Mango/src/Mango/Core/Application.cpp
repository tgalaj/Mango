#include "mgpch.h"

#include "Application.h"
#include "CVars.h"
#include "Mango/Scene/SceneManager.h"
#include "Mango/Systems/AudioSystem.h"
#include "Mango/Systems/ImGuiSystem.h"
#include "Mango/Systems/PhysicsSystem.h"
#include "Mango/Systems/RenderingSystem.h"
#include "Mango/Systems/SceneGraphSystem.h"
#include "Mango/Window/Input.h"
#include "Mango/Window/Window.h"
#include "Services.h"
#include "Timer.h"

#include "cxxopts.hpp"
#include "strutil.h"

namespace mango
{
    Application::Application(const ApplicationSettings& appSettings)
        : m_frameTime (1.0 / appSettings.maxFramerate)
    {
        MG_PROFILE_ZONE_SCOPED;

        // Set CVars
        CVarFloat CVarCameraRotationSpeed("camera.rotationSpeed", "rotation speed of the camera", 0.2f);
        CVarFloat CVarCameraMoveSpeed    ("camera.moveSpeed",     "movement speed of the camera", 10.0f);

        // Parse command line args.
        cxxopts::Options options(appSettings.windowTitle, "");

        options.add_options()
            ("w,width",      "Window width",          cxxopts::value<uint32_t>())
            ("h,height",     "Window height",         cxxopts::value<uint32_t>())
            ("f,fullscreen", "Set fullscreen window", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
            ("setCVars",     "Set console variables", cxxopts::value<std::string>());

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

        // Parse CVars
        if (optResult.count("setCVars"))
        {
            auto res = optResult["setCVars"].as<std::string>();

            auto tokens = strutil::split(res, ',');

            for (auto& token : tokens)
            {
                strutil::trim(token);
                auto kv = strutil::split(token, " ");

                auto cvarParam = CVarSystem::get()->getCVar(kv[0]);
                if (cvarParam)
                {
                    switch (cvarParam->type)
                    {
                        case CVarType::INT:
                            CVarSystem::get()->setIntCVar(kv[0], std::stoi(kv[1]));
                            break;
                        case CVarType::FLOAT:
                            CVarSystem::get()->setFloatCVar(kv[0], std::stod(kv[1]));
                            break;
                        case CVarType::STRING:
                            CVarSystem::get()->setStringCVar(kv[0], kv[1]);
                            break;
                    }
                }
            }
        }

        // Init Log, Input and GUI systems
        Input::init(m_window->getNativeWindow());
        Log::init();
        VFI::init(appSettings.commandLineArgs.argsValues[0]);

        // Add Mango Engine shaders directory to the search path
        // It shouldn't be compiled in the distribution build,
        // however, it'll work without any issues.
        // Figure out a better way to do this ???
        auto rootDir = std::filesystem::path(MG_ROOT_DIR);
        mango::VFI::addToSearchPath(rootDir / "Mango/assets/shaders");

        // Init core services
        m_eventBus     = new EventBus();
        m_sceneManager = new SceneManager();

        Services::provide(this);
        Services::provide(m_sceneManager);
        Services::provide(m_eventBus);

        // Add core systems - do not forget to configure them!        
        m_systems.add(new SceneGraphSystem());
        m_systems.add(new AudioSystem());
        m_systems.configure();

        // Physics is handled separately
        m_physicsSystem = new PhysicsSystem();
        m_physicsSystem->onInit();

        // Create Rendering and ImGui systems
        // renderingSystem and m_imGuiSystem will be deleted by m_renderingSystems
        RenderingSystem* renderingSystem = new RenderingSystem();
                         m_imGuiSystem   = new ImGuiSystem();

        m_renderingSystems.add(renderingSystem);
        m_renderingSystems.add(m_imGuiSystem);
        m_renderingSystems.configure();

        Services::provide(renderingSystem);
    }

    Application::~Application()
    {
        MG_PROFILE_ZONE_SCOPED;
        delete m_sceneManager;
        delete m_eventBus;

        VFI::deinit();
    }

    void Application::addSystem(System* system)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_systems.add(system);
        system->onInit();
    }

    double Application::getFramerate() const
    {
        return m_framerate;
    }

    void Application::start()
    {
        if (m_isRunning)
        {
            return;
        }

        MG_CORE_ASSERT_MSG(m_physicsSystem, "Mango::PhysicsSystem has be initialized at this point!");
        m_physicsSystem->onInitBodies();
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
        double physicsFreq     = 0.0;

        double startTime  = 0.0;
        double passedTime = 0.0;

        bool shouldRender = false;

        double physicsDeltaTime = glm::max(m_physicsDeltaTime, m_frameTime);

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
                MG_PROFILE_ZONE_NAMED_N(zone1, "Game Loop Update", true);
                shouldRender = true;

                unprocessedTime -= m_frameTime;
                physicsFreq     += m_frameTime;

                if (m_window->isCloseRequested())
                {
                    stop();
                }

                m_systems.updateAll(m_frameTime);
                Input::update();

                if (physicsFreq >= physicsDeltaTime)
                {   
                    MG_PROFILE_ZONE_NAMED_N(zone2, "Game Loop Physics Update", true);
                    m_physicsSystem->onUpdate(physicsDeltaTime);
                    physicsFreq = 0.0f;
                }

                if (frameCounter >= 1.0)
                {
                    m_framerate  = 1000.0 / (double)frames;
                    frames       = 0;
                    frameCounter = 0;
                }
            }

            if (shouldRender)
            {
                MG_PROFILE_ZONE_NAMED_N(zone3, "Game Loop Render", true);
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

            MG_PROFILE_FRAME_MARK;
        }
    }
}
