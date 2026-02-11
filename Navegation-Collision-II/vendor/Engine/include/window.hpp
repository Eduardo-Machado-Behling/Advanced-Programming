#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Math/Vector.hpp"
#include "Wrappers/Line.hpp"
#include "Wrappers/Point.hpp"
#include "Wrappers/Poly.hpp"

#include "Utils/StatsManager.hpp"

#include "engine.hpp"
#include "engine_api.hpp"

struct ImGuiContext;
struct ImPlotContext;

namespace Engine {

class ENGINE_API Window {
public:
  using Clock = std::chrono::high_resolution_clock;

  Window(Math::Vector<2, uint32_t> windowSize);
  Window();
  ~Window();

  bool isActivate();
  void gameloop();

  ImGuiContext *getImGuiContext() const;
  ImPlotContext *getImPlotContext() const;

  virtual void uiUpdate() = 0;

  virtual void update(double dt) = 0;
  virtual void keyCallback(int key, int scancode, int action, int mode) {}
  virtual void mouseButtonCallback(int button, int action, int mods) {}
  virtual void cursorPosCallback(double xpos, double ypos) {}
  virtual void scrollCallback(double xoffset, double yoffset) {}
  virtual void framebufferSizeCallback(int width, int height) {}

  static void staticKeyCallback(GLFWwindow *win, int key, int scancode,
                                int action, int mode);
  static void staticMouseButtonCallback(GLFWwindow *win, int button, int action,
                                        int mods);
  static void staticCursorPosCallback(GLFWwindow *win, double xpos,
                                      double ypos);
  static void staticScrollCallback(GLFWwindow *win, double xoffset,
                                   double yoffset);
  static void staticFramebufferSizeCallback(GLFWwindow *win, int width,
                                            int height);

  static void staticCharCallback(GLFWwindow *win, unsigned int codepoint);

private:
  void initOpenGL(int major, int minor);
  void init(Math::Vector<2, uint32_t> windowSize);

  // Variables
public:
protected:
  void log();
  void clearEngine();
  void terminate();

  StatsManager &sm;

  Math::Vector<2, float> m_mouse;
  Math::Vector<2, float> m_windowSize;
  char m_openglVersion[32];

  std::ofstream m_log;
  GLFWwindow *m_window;
  Engine *m_engine;
  std::chrono::time_point<Clock> m_lastTime;
  std::chrono::time_point<Clock> m_startTime;
  std::random_device rd;
  std::mt19937 gen;
};

} // namespace Engine

#endif
