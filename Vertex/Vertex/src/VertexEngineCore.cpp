#include "VertexEngineCore.h"

VertexEngineCore::VertexEngineCore(const char *title, unsigned int width, unsigned int height, Uint32 flags)
    : TICKS_PER_SECOND(25),
      SKIP_TICKS      (1000 / TICKS_PER_SECOND),
      MAX_FRAMESKIP   (5)
{
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

void VertexEngineCore::start()
{
    unsigned int next_game_tick = Time::getTimeMs();
    unsigned int loops          = 0;
    float        interpolation  = 0.0f;
    bool         quit           = false;
    SDL_Event    e;

   // SDL_StartTextInput();

    /* Loop until the user closes the window */
    while(!quit)
    {
        loops = 0;

        /* Process input */
        /* TODO: Replace switch with CoreInputManager class */
        while(SDL_PollEvent(&e) != 0)
        {
                if(e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else 
                if(e.type == SDL_KEYDOWN)
                {
                    int x = 0, y = 0;

                    switch(e.key.keysym.sym)
                    {
                        case SDLK_SPACE:
                            /* Handle keypress with current mouse position */
                            SDL_GetMouseState(&x, &y);
                            printf("Mouse pos: (%d, %d)             \r", x, y);
                            break;
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                    }
                    
                }
        }

        while (Time::getTimeMs() > next_game_tick && loops < MAX_FRAMESKIP)
        {
            /* Game update */
            /* TODO: Attach base game class methods */
            //printf("update\n");
            next_game_tick += SKIP_TICKS;
            ++loops;
        }

        /* Render */
        interpolation = float(Time::getTimeMs() + SKIP_TICKS - next_game_tick) / float(SKIP_TICKS);

        /* TODO: renderer part  */
        //printf("render with interpolation\n");
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap buffers */
        SDL_GL_SwapWindow(window->m_sdlWindow);
    }
}
