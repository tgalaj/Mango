#include "VertexEngineCore.h"


VertexEngineCore::VertexEngineCore(const char *title, unsigned int width, unsigned int height)
{
    /* Init GLEW */
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    if(GLEW_OK != glewError)
    {
        fprintf(stderr, "GLEW could not be initialized! GLEW error: %s\n", glewGetErrorString(glewError));
    }

    window = new Window(title, width, height);
}

VertexEngineCore::~VertexEngineCore()
{
    delete window;
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
    bool quit = false;
    SDL_Event e;
   // SDL_StartTextInput();

    /* Loop until the user closes the window */
    while(!quit)
    {
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

        /* update */

        /* render */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap buffers */
        SDL_GL_SwapWindow(window->m_sdlWindow);
    }
}
