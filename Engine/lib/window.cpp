#include "window.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>

#include "GLFW/glfw3.h"
#include "Math/Vector.hpp"
#include "engine.hpp"

namespace Engine {
Window::Window(Math::Vector<2, uint32_t> windowSize) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
  }

  // Using OpenGL core 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window =
      glfwCreateWindow(windowSize[0], windowSize[1], "Hello World", NULL, NULL);
  if (m_window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(m_window);

  m_engine = Engine::init((GLADloadproc)glfwGetProcAddress, windowSize);
  m_lastTime = std::chrono::high_resolution_clock::now();
}

Window::Window() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
  }

  // Using OpenGL core 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  Math::Vector<2, uint32_t> windowSize(
      {static_cast<unsigned int>(mode->width),
       static_cast<unsigned int>(mode->height)});

  m_window =
      glfwCreateWindow(windowSize[0], windowSize[1], "Hello World", NULL, NULL);
  if (m_window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(m_window);

  m_engine = Engine::init((GLADloadproc)glfwGetProcAddress, windowSize);
  m_lastTime = std::chrono::high_resolution_clock::now();
}

Window::~Window() { glfwTerminate(); }

bool Window::isActivate() { return !glfwWindowShouldClose(m_window); }

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
