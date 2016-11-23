#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
	Window(int width, int height, const std::string& title, bool debug = true);
	Window(Window&& rhs);
	Window& operator=(Window&& rhs);
	virtual ~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window& rhs) = delete;

	bool isOpen();
	void pollEvents();
	void swapBuffers();

	inline operator GLFWwindow*() const { return mWindow; }

private:
	static void initGlfw();
	static void initGlew();

	static bool sGlfwInitialized;
	static bool sGlewInitialized;

	GLFWwindow* mWindow;
};