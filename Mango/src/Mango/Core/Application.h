#pragma once

#include "SystemManager.h"

namespace mango
{
    class EventBus;
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
        EventBus* const getEventBus() const { return m_eventBus; }

        unsigned int getFPS() const;

        void start();
        void stop();

    private:
        void run();

    private:
        std::shared_ptr<Window> m_window;
        SystemManager  m_systems;
        SystemManager  m_renderingSystems;
        
        SceneManager * m_sceneManager;
        EventBus     * m_eventBus;

        double       m_frameTime;
        unsigned int m_fps;
        unsigned int m_fpsToReturn;
        bool         m_isRunning;
    };

    // Client must define this function
    Application* createApplication(ApplicationCommandLineArgs args);
}
