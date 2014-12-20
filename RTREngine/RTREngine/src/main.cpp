#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <GUI\AntTweakBar.h>

#include <cstdio>
#include <cstdlib>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

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
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
		system("pause");
		return -1;
	}

	glewExperimental = GL_TRUE;

	fprintf(stdout, "OpenGL version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(stdout, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));

	/* Init renderer options */
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.25, 0.25, 0.25, 1.0);

	/* Init GUI */
	TwInit(TW_OPENGL, nullptr);
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
		/* Render here */
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.6f, -0.4f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.6f, -0.4f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.6f, 0.f);
		glEnd();

		TwDraw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	TwTerminate();
	return 0;
}