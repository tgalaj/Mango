#pragma once

#include <entityx/entityx.h>

#include "game_logic/BaseGame.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 6

namespace mango
{
    class Core final : public entityx::EntityX
    {
    public:
        explicit Core(const std::shared_ptr<BaseGame> & game, double framerate = 60.0);
        ~Core();

        Core(const Core &) = delete;
        Core & operator=(const Core &) = delete;

        /**
         * All systems must be added before calling init().
         */
        template <typename S, typename ... Args>
        void addSystem(Args && ... args)
        {
            m_users_systems.add<S>(std::forward<Args>(args) ...);
        }

        void         init(unsigned int width, unsigned int height, const std::string & title);
        unsigned int getFPS() const;

        void start();
        void stop();

    private:
        entityx::SystemManager m_users_systems;

        std::shared_ptr<BaseGame>         m_game;

        double       m_frame_time;
        unsigned int m_fps;
        unsigned int m_fpsToReturn;
        bool         m_is_running;

        void updateSystems(entityx::TimeDelta dt);
        void updateRenderingSystems(entityx::TimeDelta dt);
        void run();
    };
}
