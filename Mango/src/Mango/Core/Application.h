#pragma once

#include "SystemManager.h"

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

    struct ApplicationSettings
    {
        uint32_t                   windowWidth;
        uint32_t                   windowHeight;
        std::string                windowTitle;
        double                     maxFramerate;
        ApplicationCommandLineArgs commandLineArgs;
    };

    class Application
    {
    public:
        Application(const ApplicationSettings& appSettings);
        virtual ~Application();

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        void addSystem(System* system);

        const std::shared_ptr<Window> getWindow() const { return m_window; }

        EventBus    * const getEventBus()    const { return m_eventBus; }
        ImGuiSystem * const getImGuiSystem() const { return m_imGuiSystem; }

        /** Returns time in ms needed to render one frame. */
        double getFramerate() const;

        void start();
        void stop();

    private:
        void run();

    private:
        std::shared_ptr<Window> m_window;
        SystemManager  m_systems;
        SystemManager  m_renderingSystems;
        
        SceneManager  * m_sceneManager;
        EventBus      * m_eventBus;
        ImGuiSystem   * m_imGuiSystem;
        PhysicsSystem * m_physicsSystem;

        double m_frameTime;
        double m_physicsDeltaTime = 1.0 / 60.0;
        double m_framerate        = 0;
        bool   m_isRunning        = false;
    };

    // Client must define this function
    Application* createApplication(ApplicationCommandLineArgs args);
}
