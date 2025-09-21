#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <chrono>

#include "engine.hpp"

namespace Engine {

class Window {
	using Clock = std::chrono::high_resolution_clock;
public:
	Window();
	~Window();

	bool isActivate();
	void gameloop();

private:
	virtual void update(double dt);

// Variables
public:

private:
	GLFWwindow* m_window;
	Engine* m_engine;
	std::chrono::time_point<Clock> m_lastTime;
};

} // Engine

#endif
