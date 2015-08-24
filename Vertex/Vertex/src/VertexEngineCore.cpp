#include "VertexEngineCore.h"

bool VertexEngineCore::quit = false;

VertexEngineCore::VertexEngineCore(const char *title, unsigned int width, unsigned int height, Uint32 flags)
    : TICKS_PER_SECOND(25),
      SKIP_TICKS      (1000 / TICKS_PER_SECOND),
      MAX_FRAMESKIP   (5)
{
    /* Init window and GL context. */
    window = new Window(title, width, height, flags);
    glClearColor(0.22f, 0.33f, 0.66f, 1.0f);

    if (window->m_isGood)
    {
        /* Init GLEW */
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();

        if (GLEW_OK != glewError)
        {
            fprintf(stderr, "Some serious errors occured. Can't initialize Vertex Engine unless these errors are fixed.\n");
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

void VertexEngineCore::start(BaseGame *game)
{
    /* Set game. */
    this->game = game;

    unsigned int next_game_tick = Time::getTimeMs();
    unsigned int loops          = 0;
    float        interpolation  = 0.0f;

    /* Loop until the user closes the window */
    while(!quit)
    {
        loops = 0;

        /* Process input */
        quit = Input::update();
        game->processInput();

        /* Game update */
        while (Time::getTimeMs() > next_game_tick && loops < MAX_FRAMESKIP)
        {
            game->update();

            next_game_tick += SKIP_TICKS;
            ++loops;
        }

        /* Render */
        interpolation = float(Time::getTimeMs() + SKIP_TICKS - next_game_tick) / float(SKIP_TICKS);

        /* TODO: renderer part  */
        //printf("render with interpolation\n");
        glClear(GL_COLOR_BUFFER_BIT);
        game->render();

        /* Swap buffers */
        SDL_GL_SwapWindow(window->m_sdlWindow);
    }
}
