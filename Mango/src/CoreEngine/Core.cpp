#include "mgpch.h"

#include "CoreEngine/Core.h"
#include "CoreEngine/CoreServices.h"
#include "CoreSystems/SceneGraphSystem.h"
#include "CoreSystems/ConsoleSystem.h"
#include "CoreSystems/CameraSystem.h"
#include "CoreSystems/GUISystem.h"
#include "CoreSystems/RenderingSystem.h"
#include "CoreSystems/FreePoseSystem.h"
#include "Utilities/Timer.h"
#include "Window/Input.h"
#include "Window/Window.h"

namespace mango
{
    Core::Core(const std::shared_ptr<BaseGame> & game, double framerate)
        : m_usersSystems(entities, events), 
          m_game        (game),
          m_frameTime   (1.0 / framerate),
          m_fps         (0),
          m_fpsToReturn (0),
          m_isRunning   (false)
    {
    }

    Core::~Core()
    {
    }

    void Core::init(uint32_t width, uint32_t height, const std::string & title)
    {
        /* Init window */
        Window::create(width, height, title);

        /* Ad{}d core systems - do not forget to update them! */
        //systems.add<AudioSystem>();
        systems.add<SceneGraphSystem>();
        systems.add<ConsoleSystem>();
        systems.add<CameraSystem>();
        systems.add<GUISystem>();
        systems.add<RenderingSystem>();
        systems.add<FreePoseSystem>();
        systems.configure();

        /* Configure user's systems */
        m_usersSystems.configure();

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
        if (m_isRunning)
        {
            return;
        }

        run();
    }

    void Core::stop()
    {
        if (!m_isRunning)
        {
            return;
        }

        m_isRunning = false;
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

        m_usersSystems.update_all(dt);
    }

    void Core::updateRenderingSystems(entityx::TimeDelta dt)
    {
        systems.update<RenderingSystem>(dt);
        systems.update<GUISystem>(dt);
    }

    void Core::run()
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

                if (Window::isCloseRequested())
                {
                    stop();
                }

                m_game->input(float(m_frameTime));
                updateSystems(m_frameTime);
                Input::update();

                if (frameCounter >= 1.0)
                {
                    frames       = 0;
                    frameCounter = 0;
                }
            }

            if (shouldRender)
            {
                /* Update Rendering and GUI systems */
                updateRenderingSystems(m_frameTime);

                Window::endFrame();
                frames++;
            }
        }
    }
}
