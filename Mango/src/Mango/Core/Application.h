#pragma once

#include "SystemManager.h"

#include <filesystem>

namespace mango
{
    class EventBus;
    class ImGuiSystem;
    class PhysicsSystem;
    class Scene;
    class SceneManager;
    class Window;

    struct ApplicationCommandLineArgs
    {
        int    argsCount;
        char** argsValues;
    };

    struct ApplicationConfiguration
    {
        uint32_t                   windowWidth   = 1920;
        uint32_t                   windowHeight  = 1080;
        std::string                windowTitle   = "Unnamed";
        double                     maxFramerate  = 200.0f;
        bool                       fullscreen    = false;
        bool                       maximized     = false;
        std::filesystem::path      projectPath   = "";

        ApplicationCommandLineArgs commandLineArgs;
    };

    class Application
    {
    public:
        Application(const ApplicationConfiguration& appConfig);
        virtual ~Application();

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        void addRuntimeSystem(System* system);
        void addEditorSystem (System* system);

        bool isRunning()        const { return m_isRunning; }
        bool isPaused()         const { return m_isPaused;  }

        const ApplicationConfiguration& getConfig() const { return m_config; }
        const ref<Window> getWindow() const { return m_window; }

        EventBus      * const getEventBus()      const { return m_eventBus;      }
        ImGuiSystem   * const getImGuiSystem()   const { return m_imGuiSystem;   }
        PhysicsSystem * const getPhysicsSystem() const { return m_physicsSystem; }

        /** Returns time in ms needed to render one frame. */
        double getFramerate() const;

        void run();
        void close();
        void step(int frames = 1);
        void setPaused(bool paused) { m_isPaused = paused; }

    private:
        ApplicationConfiguration m_config;

        ref<Window> m_window;
        SystemManager  m_runtimeSystems;
        SystemManager  m_editorSystems;
        SystemManager  m_renderingSystems;
        
        SceneManager  * m_sceneManager;
        EventBus      * m_eventBus;
        ImGuiSystem   * m_imGuiSystem;
        PhysicsSystem * m_physicsSystem;

        double m_frameTime;
        double m_physicsDeltaTime = 1.0 / 60.0;
        double m_framerate        = 0;
        int    m_stepFrames       = 0;
        bool   m_isRunning        = true;
        bool   m_isPaused         = false;
    };

    // Client must define this function
    Application* createApplication(ApplicationCommandLineArgs args);
}
