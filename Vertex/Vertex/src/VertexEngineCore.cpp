#include "VertexEngineCore.h"
#include <imgui\imgui_impl_sdl.h>

VertexEngineCore::VertexEngineCore(const char * title, unsigned int width, unsigned int height, Uint32 flags)
    : quit       (false),
      fps        (0),
      fpsToReturn(0)
{
    /* Init window and GL context. */
    window = new Window(title, width, height, flags);

    if (window->m_isGood)
    {
        /* Init GLEW */
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();

        if (GLEW_OK != glewError)
        {
            fprintf(stderr, "Some serious errors occured. Can't initialize Vertex Engine unless these errors are fixed.\n");
        }
        else
        {
            glClearColor(0.22f, 0.33f, 0.66f, 1.0f);
            renderer = new Renderer();

            CoreServices::provide(renderer);
        }

        /* Init ImGUI */
        ImGui_ImplSdl_Init(window->m_sdlWindow);
    }
    else
    {
        fprintf(stderr, "GLEW could not be initialized! GLEW error: %s\n");
    }

    CoreServices::provide(this);
}

VertexEngineCore::~VertexEngineCore()
{
    delete window;
    window = nullptr;

    delete game;
    game = nullptr;

    delete renderer;
    renderer = nullptr;
}

void VertexEngineCore::setVSync(bool enabled)
{
    /* Turn on/off VSYNC */
    if(SDL_GL_SetSwapInterval(static_cast<int>(enabled)) < 0)
    {
        fprintf(stderr, "Warning: Unable to set VSync! SDL error: %s\n", SDL_GetError());
    }
}

void VertexEngineCore::setScene(Scene * scene)
{
    this->scene = scene;
}

void VertexEngineCore::quitApp()
{
    VertexEngineCore::quit = true;
}

unsigned int VertexEngineCore::getFPS()
{
    return fpsToReturn;
}

void VertexEngineCore::start(BaseGame * game)
{
    /* Set game. */
    this->game = game;

    unsigned int last_loop_time = Time::getTimeMs();
    unsigned int now            = 0;
    unsigned int elapsed        = 0;
    unsigned int last_fps_time  = 0;

    float delta = 0.0f;

    /* Loop until the game ends */
    while(!quit)
    {
        now            = Time::getTimeMs();
        elapsed        = now - last_loop_time;
        last_loop_time = now;
        delta          = elapsed / static_cast<float>(1000);

        /* Update FPS counter */
        last_fps_time += elapsed;
        ++fps;

        if (last_fps_time >= 1000)
        {
            //SDL_SetWindowTitle(window->m_sdlWindow, (std::to_string(fps) + " FPS").c_str());
            fpsToReturn   = fps;
            last_fps_time = 0;
            fps           = 0;
        }

        /* Process input */
        quit = Input::update();
        renderer->cam->processInput(delta);
        game->processInput();

        /* Game & scene update & ImGUI */
        game->update(delta);
        scene->update();

        ImGui_ImplSdl_NewFrame(window->m_sdlWindow);
        game->onGUI();

        /* Render */
        renderer->render();

        /* Render UI */
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        ImGui::Render();

        /* Swap buffers */
        SDL_GL_SwapWindow(window->m_sdlWindow);
    }
}
