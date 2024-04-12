#include "mgpch.h"

#include "Application.h"
#include "CVars.h"
#include "Mango/Scene/SceneManager.h"
#include "Mango/Scene/SelectionManager.h"
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
    Application::Application(const ApplicationConfiguration& appSettings)
        : m_config   (appSettings),
          m_frameTime(1.0 / appSettings.maxFramerate)
    {
        MG_PROFILE_ZONE_SCOPED;

        // Set CVars
        CVarFloat CVarCameraRotationSpeed("camera.rotationSpeed", "rotation speed of the camera", 0.2f);
        CVarFloat CVarCameraMoveSpeed    ("camera.moveSpeed",     "movement speed of the camera", 10.0f);

        // Parse command line args. 
        // TODO: replace with CLI11
        cxxopts::Options options(appSettings.windowTitle, "");

        options.add_options()
            ("w,width",      "Window width",          cxxopts::value<uint32_t>())
            ("h,height",     "Window height",         cxxopts::value<uint32_t>())
            ("m,maximized",  "Open window maximized", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
            ("f,fullscreen", "Set fullscreen window", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
            ("p,project",    "Open project",          cxxopts::value<std::string>()->default_value(""))
            ("setCVars",     "Set console variables", cxxopts::value<std::string>());

        auto optResult = options.parse(appSettings.commandLineArgs.argsCount, appSettings.commandLineArgs.argsValues);

        if (optResult.count("width"))
        {
            const_cast<ApplicationConfiguration&>(appSettings).windowWidth = optResult["width"].as<uint32_t>();
        }

        if (optResult.count("height"))
        {
            const_cast<ApplicationConfiguration&>(appSettings).windowHeight = optResult["height"].as<uint32_t>();
        }

        bool maximized = false;
        if (optResult["maximized"].as<bool>() || appSettings.maximized)
        {
            maximized = true;
        }

        // Init window
        m_window = createRef<Window>(appSettings.windowWidth, appSettings.windowHeight, appSettings.windowTitle, maximized);
    
        if (optResult["fullscreen"].as<bool>() || appSettings.fullscreen)
        {
            m_window->setFullscreen(true);
        }

        // Set project path if specified in cmd line args
        if (optResult.count("project"))
        {
            auto projectPath          = optResult["project"].as<std::string>();
                 m_config.projectPath = projectPath;
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

        m_selectionManager = SelectionManager::create();

        // Add Mango Engine shaders directory to the search path
        // It shouldn't be compiled in the distribution build,
        // however, it'll work without any issues.
        // Figure out a better way to do this ???
        auto rootDir = std::filesystem::path(MG_ROOT_DIR);
        mango::VFI::addToSearchPath(rootDir / "Mango/resources");
        mango::VFI::addToSearchPath(rootDir / "Mango/resources/shaders");

        // Init core services
        AssetManager::initDefaultAssets();

        m_eventBus     = new EventBus();
        m_sceneManager = new SceneManager();

        Services::provide(this);
        Services::provide(m_sceneManager);
        Services::provide(m_eventBus);

        // Add core systems - do not forget to configure them!        
        m_runtimeSystems.add(new SceneGraphSystem());
        m_runtimeSystems.add(new AudioSystem());
        m_runtimeSystems.configure();

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

    void Application::addRuntimeSystem(System* system)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_runtimeSystems.add(system);
        system->onInit();
    }

    void Application::addEditorSystem(System* system)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_editorSystems.add(system);
        system->onInit();
    }

    double Application::getFramerate() const
    {
        return m_framerate;
    }

    void Application::close()
    {
        m_isRunning = false;
    }

    void Application::step(int frames /*= 1*/)
    {
        m_stepFrames = frames;
    }

    void Application::run()
    {
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
                    close();
                }

                if (physicsFreq >= physicsDeltaTime)
                {
                    MG_PROFILE_ZONE_NAMED_N(zone2, "Game Loop Physics Update", true);
                    if (!m_isPaused || m_stepFrames > 0)
                    {
                        m_physicsSystem->onUpdate(physicsDeltaTime);
                        physicsFreq = 0.0f;
                    }
                }

                if (!m_isPaused || m_stepFrames-- > 0)
                {
                    m_runtimeSystems.updateAll(m_frameTime);
                }

                m_editorSystems.updateAll(m_frameTime);
                Input::update();

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
                    for (auto& system : m_runtimeSystems)
                    {
                        system->onGui();
                    }
                    for (auto& system : m_editorSystems)
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
