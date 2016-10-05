#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

int main()
{
	try {
		if (!glfwInit())
			throw GLException() << "Failed to initialize GLFW.";

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Example", NULL, NULL);
		glfwMakeContextCurrent(window);

		int glewInitOk = glewInit();
		if (glewInitOk != GLEW_OK)
			throw GLException() << glewGetErrorString(glewInitOk);

		Shader shader = Shader::fromFile("vertex.glsl", "fragment.glsl");
		shader.use();

		while (!glfwWindowShouldClose(window))
		{
			// render
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwTerminate();
		return 0;
	} catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		while (true);
		return 1;
	}
}