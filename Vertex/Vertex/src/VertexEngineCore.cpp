#include "VertexEngineCore.h"

bool         VertexEngineCore::quit = false;
unsigned int VertexEngineCore::fps    = 0;

VertexEngineCore::VertexEngineCore(const char * title, unsigned int width, unsigned int height, Uint32 flags)
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
    }
    else
    {
        fprintf(stderr, "GLEW could not be initialized! GLEW error: %s\n");
    }
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

void VertexEngineCore::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void VertexEngineCore::setVSync(bool enabled)
{
    /* Turn on/off VSYNC */
    if(SDL_GL_SetSwapInterval(static_cast<int>(enabled)) < 0)
    {
        fprintf(stderr, "Warning: Unable to set VSync! SDL error: %s\n", SDL_GetError());
    }
}

void VertexEngineCore::quitApp()
{
    VertexEngineCore::quit = true;
}

unsigned int VertexEngineCore::getFPS()
{
    return fps;
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
            last_fps_time = 0;
            fps = 0;
        }

        /* Process input */
        quit = Input::update();
        game->processInput();

        /* Game update */
        game->update(delta);

        /* Render */
        renderer->render();
        game->render();

        /* Swap buffers */
        SDL_GL_SwapWindow(window->m_sdlWindow);
    }
}
