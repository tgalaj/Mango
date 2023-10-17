#include "core_engine/Core.h"
#include "core_engine/CoreServices.h"
#include "core_systems/SceneGraphSystem.h"
#include "core_systems/ConsoleSystem.h"
#include "core_systems/CameraSystem.h"
#include "core_systems/GUISystem.h"
#include "core_systems/RenderingSystem.h"
#include "core_systems/FreePoseSystem.h"
#include "utilities/Timer.h"
#include "window/Input.h"
#include "window/Window.h"

namespace mango
{
    Core::Core(const std::shared_ptr<BaseGame> & game, double framerate)
        : m_users_systems(entities, events), 
          m_game(game),
          m_frame_time(1.0 / framerate),
          m_fps(0),
          m_fpsToReturn(0),
          m_is_running(false)
    {
    }

    Core::~Core()
    {
    }

    void Core::init(unsigned int width, unsigned int height, const std::string & title)
    {
        /* Init window */
        Window::createWindow(width, height, title);

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
        m_users_systems.configure();

        /* Set up Core Services */
        CoreServices::provide(this);
        CoreServices::provide(systems.system<RenderingSystem>().get());

        /* Register game's methods */
        systems.system<GUISystem>()->registerGame(m_game);

        m_game->init();
    }

    unsigned int Core::getFPS() const
    {
        return m_fpsToReturn;
    }

    void Core::start()
    {
        if (m_is_running)
        {
            return;
        }

        run();
    }

    void Core::stop()
    {
        if (!m_is_running)
        {
            return;
        }

        m_is_running = false;
    }

    void Core::updateSystems(entityx::TimeDelta dt)
    {
        /** 
         * Update core engine systems in specific order 
         */
        //systems.update<AudioSystem>();
        systems.update<ConsoleSystem>(dt);
        systems.update<FreePoseSystem>(dt);
        systems.update<CameraSystem>(dt);
        systems.update<SceneGraphSystem>(dt);

        m_users_systems.update_all(dt);
    }

    void Core::updateRenderingSystems(entityx::TimeDelta dt)
    {
        systems.update<RenderingSystem>(dt);
        systems.update<GUISystem>(dt);
    }

    void Core::run()
    {
        m_is_running = true;

        int frames = 0;
        double frame_counter = 0.0;

        double last_time = Timer::getTime();
        double unprocessed_time = 0.0;

        double start_time = 0.0;
        double passed_time = 0.0;

        bool should_render = false;

        while (m_is_running)
        {
            should_render = false;

            start_time = Timer::getTime();
            passed_time = start_time - last_time;

            last_time = start_time;

            unprocessed_time += passed_time;
            frame_counter += passed_time;

            while (unprocessed_time > m_frame_time)
            {
                should_render = true;

                unprocessed_time -= m_frame_time;

                if (Window::isCloseRequested())
                {
                    stop();
                }

                m_game->input(float(m_frame_time));
                updateSystems(m_frame_time);
                Input::update();

                if (frame_counter >= 1.0)
                {
                    #ifdef _DEBUG
                    //std::cout << 1000.0 / (double)frames << std::endl;
                    #endif

                    frames = 0;
                    frame_counter = 0;
                }
            }

            if (should_render)
            {
                /* Update Rendering and GUI systems */
                updateRenderingSystems(m_frame_time);

                Window::endFrame();
                frames++;
            }
        }
    }
}
