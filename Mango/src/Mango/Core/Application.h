#pragma once

#include "Log.h"
#include "Mango/Events/Event.h"
#include "Mango/GameLogic/BaseGame.h"
#include <entityx/entityx.h>

namespace mango
{
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

    class Application : public entityx::EntityX
    {
    public:
        Application(const ApplicationSettings& appSettings);
        virtual ~Application();

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        /**
         * All systems must be added before calling init().
        */
        template <typename S, typename ... Args>
        void addSystem(Args && ... args)
        {
            m_usersSystems.add<S>(std::forward<Args>(args) ...);
        }

        void setGame(const std::shared_ptr<BaseGame>& game);
        const std::shared_ptr<BaseGame>& getGame() { return m_game; }

        std::shared_ptr<EventBus>& getEventBus() { return m_eventBus; }

        unsigned int getFPS() const;

        void start();
        void stop();

    private:
        void updateSystems(entityx::TimeDelta dt);
        void updateRenderingSystems(entityx::TimeDelta dt);
        void run();

    private:
        entityx::SystemManager m_usersSystems;
        std::shared_ptr<BaseGame> m_game;

        double       m_frameTime;
        unsigned int m_fps;
        unsigned int m_fpsToReturn;
        bool         m_isRunning;

        std::shared_ptr<EventBus> m_eventBus;
    };

    // Client must define this function
    Application* createApplication(ApplicationCommandLineArgs args);
}
