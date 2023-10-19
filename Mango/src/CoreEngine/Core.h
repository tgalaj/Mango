#pragma once

#include "GameLogic/BaseGame.h"
#include <entityx/entityx.h>

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 6

namespace mango
{
    class Core final : public entityx::EntityX
    {
    public:
        explicit Core(const std::shared_ptr<BaseGame> & game, double framerate = 60.0);
        ~Core();

        Core(const Core &)             = delete;
        Core & operator=(const Core &) = delete;

        /**
         * All systems must be added before calling init().
         */
        template <typename S, typename ... Args>
        void addSystem(Args && ... args)
        {
            m_usersSystems.add<S>(std::forward<Args>(args) ...);
        }

        void init(uint32_t width, uint32_t height, const std::string & title);
        unsigned int getFPS() const;

        void start();
        void stop();

    private:
        void updateSystems         (entityx::TimeDelta dt);
        void updateRenderingSystems(entityx::TimeDelta dt);
        void run();

    private:
        entityx::SystemManager    m_usersSystems;
        std::shared_ptr<BaseGame> m_game;

        double       m_frameTime;
        unsigned int m_fps;
        unsigned int m_fpsToReturn;
        bool         m_isRunning;
    };
}
