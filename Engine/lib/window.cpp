#include "window.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>

#include "GLFW/glfw3.h"
#include "Math/Vector.hpp"
#include "engine.hpp"

namespace Engine {

Window::Window(Math::Vector<2, uint32_t> windowSize) {
  initOpenGL(4, 3); // Request OpenGL 4.3
  init(windowSize);
}

Window::Window() {
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

  if (!m_log.is_open()) {
    m_log.open("log.csv");
    m_log << "time,mouseX,mouseY,mouseClicks,uuid,uuidType,fps,drawCalls,"
             "entities,pointsAmount,linesAmount,polyAmount\n";
  }

  auto c = m_engine->count();

  m_state.pointAmount = c.points;
  m_state.linesAmount = c.lines;
  m_state.polyAmount = c.polys;

  m_log << time << ',' << m_state.mouseX << ',' << m_state.mouseY << ','
        << m_state.mouseClickAmount << ',' << m_state.uuid << ','
        << m_state.uuidType << ',' << m_state.fps << ',' << m_state.drawCalls
        << ',' << m_state.entities << ',' << m_state.pointAmount << ','
        << m_state.linesAmount << ',' << m_state.polyAmount << '\n';

  m_state.reset();
}

void Window::gameloop() {
  auto now = Clock::now();
  double dt = std::chrono::duration<double>(now - m_lastTime).count();

  glfwPollEvents();
  glfwGetCursorPos(m_window, &m_state.mouseX, &m_state.mouseY);
  m_state.fps = 1.0 / dt;
  m_state.entities = m_engine->entities();
  m_state.drawCalls = m_engine->drawCalls();

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
    self->m_state.mouseClickAmount += 1;
    glfwGetCursorPos(self->m_window, &self->m_state.mouseX,
                     &self->m_state.mouseY);
    int w, h;
    glfwGetWindowSize(self->m_window, &w, &h);
    self->m_state.uuid = self->m_engine->lookupObjectUUID(
        self->m_state.mouseX, h - self->m_state.mouseY);
    self->m_state.uuidType = self->m_engine->getType(self->m_state.uuid);
    self->log();
  }

  if (self)
    self->mouseButtonCallback(button, action, mods);
}

void Window::staticCursorPosCallback(GLFWwindow *win, double xpos,
                                     double ypos) {
  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self)
    self->cursorPosCallback(xpos, ypos);
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
  if (self)
    self->framebufferSizeCallback(width, height);
}
} // namespace Engine
