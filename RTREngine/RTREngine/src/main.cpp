#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GUI\AntTweakBar.h>

#include <cstdio>
#include <cstdlib>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 5

static void mouseButtonCB(GLFWwindow* window, int button, int action, int mods)
{
	if (!TwEventMouseButtonGLFW(button, action))
	{
	}
}

static void mousePosCB(GLFWwindow* window, double mouseX, double mouseY)
{
	if (!TwEventMousePosGLFW(static_cast<int>(mouseX), static_cast<int>(mouseY)))
	{ 
	}
}

static void mouseScrollCB(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!TwEventMouseWheelGLFW(yoffset))
	{
	}
}

static void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (!TwEventCharGLFW(key, action))
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwDestroyWindow(window);
		}

	}
}

static void onResize(GLFWwindow* window, int w, int h)
{
	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	//update proj matrix
	float ascpetRatio = static_cast<float>(w) / static_cast<float>(h);

	TwWindowSize(w, h);
}

int main(void)
{
	GLFWwindow* window = nullptr;
	
	/* Initialize the library */
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW init failed!\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		std::string glVersion = std::to_string(GL_VERSION_MAJOR) + "." + std::to_string(GL_VERSION_MINOR);
		fprintf(stderr, "Window creation failed! OpenGL %s not supported!\n", glVersion.c_str());
		system("pause");
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
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

	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)mouseButtonCB);
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)mousePosCB);
	glfwSetScrollCallback(window, (GLFWscrollfun)mouseScrollCB);
	glfwSetKeyCallback(window, (GLFWkeyfun)keyCB);
	glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)onResize);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		TwDraw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	TwTerminate();
	return 0;
}