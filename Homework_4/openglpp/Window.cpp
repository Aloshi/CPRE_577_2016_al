#include "Window.h"
#include "GLException.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

bool Window::sGlewInitialized = false;
bool Window::sGlfwInitialized = false;

void APIENTRY printGlDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
	                        GLsizei length, const GLchar* message, const void* userParam)
{
	const char* type_string = "GL";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		type_string = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		type_string = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		type_string = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		type_string = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		type_string = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		type_string = "OTHER";
		break;
	}

	std::cout << type_string << ": " << message << std::endl;
}

void Window::initGlfw()
{
	if (!sGlfwInitialized) {
		if (!glfwInit())
			throw GLException() << "Failed to initialize GLFW.";

		sGlfwInitialized = true;
	}
}

void Window::initGlew()
{
	if (!sGlewInitialized) {
		glewExperimental = GL_TRUE;
		int glewInitOk = glewInit();
		if (glewInitOk != GLEW_OK)
			throw GLException() << "Failed to initialize GLEW: " << glewGetErrorString(glewInitOk);

		sGlewInitialized = true;
	}
}

Window::Window(int width, int height, const std::string& title, bool debug)
	: mWindow(NULL)
{
	initGlfw();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug ? 1 : 0);
	mWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	if (mWindow == NULL) {
		throw GLException() << "Failed to create GLFW window.";
	}

	glfwMakeContextCurrent(mWindow);

	initGlew();

	if (debug) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(printGlDebugOutput, NULL);
	}
}



Window::Window(Window&& rhs)
{
	mWindow = rhs.mWindow;
	rhs.mWindow = NULL;
}

Window& Window::operator=(Window&& rhs)
{
	if (mWindow) {
		glfwDestroyWindow(mWindow);
	}

	mWindow = rhs.mWindow;
	rhs.mWindow = NULL;
	return *this;
}

Window::~Window()
{
	if (mWindow) {
		glfwDestroyWindow(mWindow);
	}
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::swapBuffers()
{
	glfwSwapBuffers(mWindow);
}

bool Window::isOpen()
{
	return mWindow ? !glfwWindowShouldClose(mWindow) : false;
}