#include "window.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <variant>

#include "GLFW/glfw3.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "implot.h"

#include "Math/Vector.hpp"
#include "Objects/ObjectUUID.hpp"
#include "engine.hpp"

extern IMGUI_IMPL_API void ImGui_ImplGlfw_KeyCallback(GLFWwindow *window,
                                                      int key, int scancode,
                                                      int action, int mods);
extern IMGUI_IMPL_API void
ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow *window, int button, int action,
                                   int mods);
extern IMGUI_IMPL_API void ImGui_ImplGlfw_CursorPosCallback(GLFWwindow *window,
                                                            double x, double y);
extern IMGUI_IMPL_API void ImGui_ImplGlfw_ScrollCallback(GLFWwindow *window,
                                                         double xoffset,
                                                         double yoffset);

namespace Engine {

Window::Window(Math::Vector<2, uint32_t> windowSize)
    : gen(rd()), sm(StatsManager::get()) {
  initOpenGL(4, 3);
  init(windowSize);
}

Window::Window() : gen(rd()), sm(StatsManager::get()) {
  initOpenGL(4, 3);
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  Math::Vector<2, uint32_t> windowSize(
      {static_cast<unsigned int>(mode->width),
       static_cast<unsigned int>(mode->height)});
  init(windowSize);
}

Window::~Window() {
  ImGui_ImplOpenGL3_Shutdown();

  ImGui_ImplGlfw_Shutdown();

  if (ImPlot::GetCurrentContext()) {
    ImPlot::DestroyContext();
  }

  if (ImGui::GetCurrentContext()) {
    ImGui::DestroyContext();
  }

  glfwDestroyWindow(m_window);
  glfwTerminate();
}

bool Window::isActivate() { return !glfwWindowShouldClose(m_window); }

void Window::clearEngine() { m_engine->clear(); }

void Window::terminate() { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }

void Window::log() {
  auto now = Clock::now();
  double time = std::chrono::duration<double>(now - m_startTime).count();

  auto c = m_engine->count();
  sm.set("pointAmount", c.points);
  sm.set("linesAmount", c.lines);
  sm.set("polyAmount", c.polys);
  sm.set("time", time);

  sm.log();

  sm.set("uuid", 0ull);
  sm.set("uuidType", 0ull);
}

ImGuiContext *Window::getImGuiContext() const {
  return ImGui::GetCurrentContext();
}

ImPlotContext *Window::getImPlotContext() const {
  return ImPlot::GetCurrentContext();
}

void Window::gameloop() {
  auto now = Clock::now();
  double dt = std::chrono::duration<double>(now - m_lastTime).count();

  double x, y;
  glfwGetCursorPos(m_window, &x, &y);

  sm.set("mouseX", x);
  sm.set("mouseY", y);

  sm.set("fps", 1.0 / dt);
  sm.set("entities", m_engine->entities());
  sm.set("drawCalls", m_engine->drawCalls());
  log();
  glfwMakeContextCurrent(m_window);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  this->uiUpdate();

  this->update(dt);
  m_engine->draw();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(m_window);
  glfwPollEvents();

  m_lastTime = now;
}

void Window::initOpenGL(int major, int minor) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  std::snprintf(m_openglVersion, sizeof(m_openglVersion), "#version %d%d0",
                major, minor);
}

void Window::init(Math::Vector<2, uint32_t> windowSize) {

  m_window = glfwCreateWindow(windowSize[0], windowSize[1], "Engine Window",
                              NULL, NULL);

  m_windowSize[0] = windowSize[0];
  m_windowSize[1] = windowSize[1];

  if (m_window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(0);

  m_engine = Engine::init((GLADloadproc)glfwGetProcAddress, windowSize);
  m_lastTime = std::chrono::high_resolution_clock::now();
  m_startTime = std::chrono::high_resolution_clock::now();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(m_window, false);
  ImGui_ImplOpenGL3_Init(m_openglVersion);

  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(m_window, &Window::staticKeyCallback);
  glfwSetMouseButtonCallback(m_window, &Window::staticMouseButtonCallback);
  glfwSetCursorPosCallback(m_window, &Window::staticCursorPosCallback);
  glfwSetScrollCallback(m_window, &Window::staticScrollCallback);
  glfwSetFramebufferSizeCallback(m_window,
                                 &Window::staticFramebufferSizeCallback);
  glfwSetCharCallback(m_window, &Window::staticCharCallback);

  sm.set("mouseX", 0.0);
  sm.set("mouseY", 0.0);
  sm.set("mouseClickAmount", 0llu);
  sm.set("uuid", 0llu);
  sm.set("uuidType", 0llu);
  sm.set("fps", 0.0);
  sm.set("drawCalls", 0llu);
  sm.set("entities", 0llu);
  sm.set("pointAmount", 0llu);
  sm.set("linesAmount", 0llu);
  sm.set("polyAmount", 0llu);
  sm.set("time", 0.0);
}

void Window::staticKeyCallback(GLFWwindow *win, int key, int scancode,
                               int action, int mode) {

  ImGui_ImplGlfw_KeyCallback(win, key, scancode, action, mode);

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureKeyboard) {
    return;
  }

  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self)
    self->keyCallback(key, scancode, action, mode);
}

void Window::staticMouseButtonCallback(GLFWwindow *win, int button, int action,
                                       int mods) {

  ImGui_ImplGlfw_MouseButtonCallback(win, button, action, mods);

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse) {
    return;
  }

  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (!self)
    return;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    self->sm.set("mouseClickAmount",
                 std::get<uint64_t>(self->sm.get("mouseClickAmount").value()) +
                     1);

    double x, y;
    glfwGetCursorPos(self->m_window, &x, &y);
    int w, h;
    glfwGetWindowSize(self->m_window, &w, &h);

    Objects::ObjectUUID::UUID uuid = self->m_engine->lookupObjectUUID(x, y);
    self->sm.set("uuid", uuid);
    self->sm.set("uuidType", self->m_engine->getType(uuid));

    self->sm.set("mouseX", x);
    self->sm.set("mouseY", y);
    self->log();
  }

  self->mouseButtonCallback(button, action, mods);
}

void Window::staticCursorPosCallback(GLFWwindow *win, double xpos,
                                     double ypos) {

  ImGui_ImplGlfw_CursorPosCallback(win, xpos, ypos);

  Window *self = static_cast<Window *>(glfwGetWindowUserPointer(win));
  if (self) {
    self->m_mouse[0] = xpos;
    self->m_mouse[1] = ypos;
    self->cursorPosCallback(xpos, ypos);
  }
}

void Window::staticScrollCallback(GLFWwindow *win, double xoffset,
                                  double yoffset) {

  ImGui_ImplGlfw_ScrollCallback(win, xoffset, yoffset);

  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse) {
    return;
  }

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

void Window::staticCharCallback(GLFWwindow *win, unsigned int codepoint) {

  ImGui_ImplGlfw_CharCallback(win, codepoint);
}
} // namespace Engine
