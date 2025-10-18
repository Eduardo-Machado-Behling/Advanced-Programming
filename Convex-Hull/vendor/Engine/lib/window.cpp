#include "window.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <variant>

#include "GLFW/glfw3.h"
#include "Math/Vector.hpp"
#include "Objects/ObjectUUID.hpp"
#include "engine.hpp"

namespace Engine {

Window::Window(Math::Vector<2, uint32_t> windowSize) : gen(rd()) {
  initOpenGL(4, 3); // Request OpenGL 4.3
  init(windowSize);
}

Window::Window() : gen(rd()) {
  initOpenGL(4, 3); // Request OpenGL 4.3
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  Math::Vector<2, uint32_t> windowSize(
      {static_cast<unsigned int>(mode->width),
       static_cast<unsigned int>(mode->height)});
  init(windowSize);
}

Window::~Window() { glfwTerminate(); }

bool Window::isActivate() { return !glfwWindowShouldClose(m_window); }

void Window::log() {
  auto now = Clock::now();
  double time = std::chrono::duration<double>(now - m_startTime).count();
  auto &headers = m_state.headers();

  if (!m_log.is_open()) {
    m_log.open("log.csv");

    m_log << "time,";
    for (size_t i = 0; i < headers.size(); i++) {
      m_log << headers[i];
      if (i == headers.size() - 1) {
        m_log << '\n';
      } else {
        m_log << ',';
      }
    }
  }

  auto c = m_engine->count();

  m_state.get("pointAmount") = c.points;
  m_state.get("linesAmount") = c.lines;
  m_state.get("polyAmount") = c.polys;

  m_log << time << ',';
  for (size_t i = 0; i < headers.size(); i++) {
    auto &var = m_state.get(headers[i].c_str());
    std::visit([&](const auto &value) { m_log << value; }, var);

    if (i == headers.size() - 1) {
      m_log << '\n';
    } else {
      m_log << ',';
    }
  }

  m_state.reset();
}

void Window::gameloop() {
  auto now = Clock::now();
  double dt = std::chrono::duration<double>(now - m_lastTime).count();

  glfwPollEvents();
  double x, y;
  glfwGetCursorPos(m_window, &x, &y);

  m_state.get("mouseX") = x;
  m_state.get("mouseY") = y;

  m_state.get("fps") = 1.0 / dt;
  m_state.get("entities") = m_engine->entities();
  m_state.get("drawCalls") = m_engine->drawCalls();

  log();
  this->update(dt);
  m_engine->draw();

  glfwSwapBuffers(m_window);
  m_lastTime = now;
}

void Window::initOpenGL(int major, int minor) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    // Consider throwing an exception here
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::init(Math::Vector<2, uint32_t> windowSize) {

  m_window = glfwCreateWindow(windowSize[0], windowSize[1], "Engine Window",
                              NULL, NULL);

  m_windowSize[0] = windowSize[0];
  m_windowSize[1] = windowSize[1];

  if (m_window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    // Consider throwing an exception
  }

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(0);

  m_engine = Engine::init((GLADloadproc)glfwGetProcAddress, windowSize);
  m_lastTime = std::chrono::high_resolution_clock::now();
  m_startTime = std::chrono::high_resolution_clock::now();

  // --- REGISTER ALL CALLBACKS ---
  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(m_window, &Window::staticKeyCallback);
  glfwSetMouseButtonCallback(m_window, &Window::staticMouseButtonCallback);
  glfwSetCursorPosCallback(m_window, &Window::staticCursorPosCallback);
  glfwSetScrollCallback(m_window, &Window::staticScrollCallback);
  glfwSetFramebufferSizeCallback(m_window,
                                 &Window::staticFramebufferSizeCallback);
}

void Window::staticKeyCallback(GLFWwindow *win, int key, int scancode,
                               int action, int mode) {
  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self)
    self->keyCallback(key, scancode, action, mode);
}

void Window::staticMouseButtonCallback(GLFWwindow *win, int button, int action,
                                       int mods) {
  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    std::get<2>(self->m_state.get("mouseClickAmount")) += 1;

    double x, y;
    glfwGetCursorPos(self->m_window, &x, &y);
    int w, h;
    glfwGetWindowSize(self->m_window, &w, &h);

    Objects::ObjectUUID::UUID uuid = self->m_engine->lookupObjectUUID(x, y);
    self->m_state.get("uuid") = uuid;
    self->m_state.get("uuidType") = self->m_engine->getType(uuid);

    self->m_state.get("mouseX") = x;
    self->m_state.get("mouseY") = y;
    self->log();
  }

  if (self)
    self->mouseButtonCallback(button, action, mods);
}

void Window::staticCursorPosCallback(GLFWwindow *win, double xpos,
                                     double ypos) {
  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self) {
    self->m_mouse[0] = xpos;
    self->m_mouse[1] = ypos;
    self->cursorPosCallback(xpos, ypos);
  }
}

void Window::staticScrollCallback(GLFWwindow *win, double xoffset,
                                  double yoffset) {
  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self)
    self->scrollCallback(xoffset, yoffset);
}

void Window::staticFramebufferSizeCallback(GLFWwindow *win, int width,
                                           int height) {
  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self) {
    self->m_windowSize[0] = width;
    self->m_windowSize[1] = height;

    self->m_engine->setWinSize(self->m_windowSize);
    self->framebufferSizeCallback(width, height);
  }
}
} // namespace Engine
