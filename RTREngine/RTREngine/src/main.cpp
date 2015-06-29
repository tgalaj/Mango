#include <GL\glew.h>
#include <GUI\AntTweakBar.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>
#include <gl\GLU.h>

#include <cstdio>
#include <cstdlib>

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
	SDL_Window*  window        = nullptr;
    SDL_Surface* screenSurface = nullptr;
	
	/* Initialize the SDL library */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL error :%s\n", SDL_GetError());
        system("pause");
		return -1;
	}

	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);*/

	/* Create a windowed mode window and its OpenGL context */
    window = SDL_CreateWindow("Hello SDL!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window)
	{
        fprintf(stderr, "Window could not be created! SDL error :%s\n", SDL_GetError());

		/*std::string glVersion = std::to_string(GL_VERSION_MAJOR) + "." + std::to_string(GL_VERSION_MINOR);
		fprintf(stderr, "Window creation failed! OpenGL %s not supported!\n", glVersion.c_str());*/
		system("pause");
		return -1;
	}

    /* Get window surface */
    screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    //Update the surface
    SDL_UpdateWindowSurface(window);

    //Wait two seconds
    SDL_Delay(2000);

	/* Set input mode */
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	/* Make the window's context current */
	//glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//glfwTerminate();
		system("pause");
		return -1;
	}

	fprintf(stdout, "OpenGL version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(stdout, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));

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
	
	/* Loop until the user closes the window */
	//while (!glfwWindowShouldClose(window))
	//{
	//	glClear(GL_COLOR_BUFFER_BIT);

	//	TwDraw();

	//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//		printf("Guzik W!\n");

	//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//		printf("Guzik A!\n");

	//	/* Swap front and back buffers */
	//	glfwSwapBuffers(window);

	//	/* Poll for and process events */
	//	glfwPollEvents();
	//}

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

	TwTerminate();
	return 0;
}