#include <GL\glew.h>
#include <GUI\AntTweakBar.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>
#include <gl\GLU.h>
#include <GL\glew.h>

#include <GUI\imgui_impl_sdl2.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 5

void drawGUI(SDL_Window* window)
{
    bool done = ImGui_ImplSdl_NewFrame(window);
    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }

    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
}

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

    ImGui_ImplSDL2_Init(window);

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

        drawGUI(window);
        TwDraw();

        /* Swap buffers */
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplSDL2_Shutdown();
    TwTerminate();

    SDL_StopTextInput();

    /* Destroy window */
    SDL_DestroyWindow(window);
    window = nullptr;

    /* Quit SDL subsystems */
    SDL_Quit();
	return 0;
}