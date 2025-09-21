#include "window.hpp"

#include <chrono>
#include <iostream>

#include "GLFW/glfw3.h"
#include "engine.hpp"

namespace Engine {
Window::Window() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
  }

  // Using OpenGL core 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);

  m_engine = Engine::get();
  m_lastTime = std::chrono::high_resolution_clock::now();
}

Window::~Window() { glfwTerminate(); }

bool Window::isActivate() {
	return glfwWindowShouldClose(m_window);
}

void Window::gameloop() {
	auto now = Clock::now();
	double dt = std::chrono::duration<double>(now - m_lastTime).count();

	glfwPollEvents();
	this->update(dt);
	m_engine->draw();

	glfwSwapBuffers(m_window);
	m_lastTime = now;
}

} // namespace Engine
