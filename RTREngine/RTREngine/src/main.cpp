#include <GL\glew.h>
#include <GUI\AntTweakBar.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>
#include <gl\GLU.h>
#include <GL\glew.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 5

//static void mouseButtonCB(GLFWwindow* window, int button, int action, int mods)
//{
//	if (!TwEventMouseButtonGLFW(button, action))
//	{
//	}
//}
//
//static void mousePosCB(GLFWwindow* window, double mouseX, double mouseY)
//{
//	if (!TwEventMousePosGLFW(static_cast<int>(mouseX), static_cast<int>(mouseY)))
//	{ 
//	}
//}
//
//static void mouseScrollCB(GLFWwindow* window, double xoffset, double yoffset)
//{
//	if (!TwEventMouseWheelGLFW(yoffset))
//	{
//	}
//}
//
//static void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (!TwEventCharGLFW(key, action))
//	{
//		if (key == GLFW_KEY_ESCAPE)
//		{
//			glfwDestroyWindow(window);
//		}
//
//	}
//}
//
//static void onResize(GLFWwindow* window, int w, int h)
//{
//	if (h == 0)
//		h = 1;
//
//	glViewport(0, 0, w, h);
//
//	//update proj matrix
//	float ascpetRatio = static_cast<float>(w) / static_cast<float>(h);
//
//	TwWindowSize(w, h);
//}

int main(int argc, char* args[])
{
    SDL_GLContext  glContext     = nullptr;
    SDL_Window*    window        = nullptr;

    /* Initialize the SDL library */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL error :%s\n", SDL_GetError());

        system("pause");
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_VERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_VERSION_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    /* Create a windowed mode window and its OpenGL context */
    window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(stderr, "Window could not be created! SDL error :%s\n", SDL_GetError());

        std::string glVersion = std::to_string(GL_VERSION_MAJOR) + "." + std::to_string(GL_VERSION_MINOR);
        fprintf(stderr, "Window creation failed! OpenGL %s not supported!\n", glVersion.c_str());

        system("pause");
        return -1;
    }

    /* Create GL context */
    glContext = SDL_GL_CreateContext(window);

    if(!glContext)
    {
        fprintf(stderr, "OpenGL context could not be created! SDL error: %s\n", SDL_GetError());

        system("pause");
        return -1;
    }

    /* Init GLEW */
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();

    if(GLEW_OK != glewError)
    {
        fprintf(stderr, "GLEW could not be initialized! GLEW error: %s\n", glewGetErrorString(glewError));

        system("pause");
        return -1;
    }

    fprintf(stdout, "OpenGL version: %s\n", glGetString(GL_VERSION));
    fprintf(stdout, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
    fprintf(stdout, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));

    /* Turn on VSYNC */
    if(SDL_GL_SetSwapInterval(1) < 0)
    {
        fprintf(stderr, "Warning: Unable to set VSync! SDL error: %s\n", glewGetErrorString(glewError));
    }

    /* Init renderer options */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.25, 0.25, 0.25, 1.0);

    /* Init GUI */
    TwInit(TW_OPENGL_CORE, nullptr);
    TwWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    TwBar* myBar = TwNewBar("Hello bar");

    int my_var = 10;
    TwAddVarRW(myBar, "Var name", TW_TYPE_INT32, &my_var, "");

    /*glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)mouseButtonCB);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)mousePosCB);
    glfwSetScrollCallback(window, (GLFWscrollfun)mouseScrollCB);
    glfwSetKeyCallback(window, (GLFWkeyfun)keyCB);
    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)onResize);*/
    
    bool quit = false;
    SDL_Event e;
    SDL_StartTextInput();

    /* Loop until the user closes the window */
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            bool handled = TwEventSDL(&e, SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
            if(!handled)
            {
                if(e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else 
                if(e.type == SDL_TEXTINPUT)
                {
                    /* Handle keypress with current mouse position */
                    int x = 0, y = 0;
                    SDL_GetMouseState(&x, &y);
                    printf("Mouse pos: (%d, %d)             \r", x, y);
                }
            }
        }

        /* update */

        /* render */
        glClear(GL_COLOR_BUFFER_BIT);

        TwDraw();

        /* Swap buffers */
        SDL_GL_SwapWindow(window);
    }

    TwTerminate();

    SDL_StopTextInput();

    /* Destroy window */
    SDL_DestroyWindow(window);
    window = nullptr;

    /* Quit SDL subsystems */
    SDL_Quit();
	return 0;
}