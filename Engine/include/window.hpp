#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <fstream>

#include "Math/Vector.hpp"

#include "engine.hpp"
#include "engine_api.hpp"

namespace Engine {

class ENGINE_API Window {
  using Clock = std::chrono::high_resolution_clock;

public:
  Window(Math::Vector<2, uint32_t> windowSize);
  Window();
  ~Window();

  bool isActivate();
  void gameloop();
  void log();

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

private:
  struct LogState {
    double mouseX = 0;
    double mouseY = 0;
    uint32_t mouseClickAmount = 0;
    uint32_t uuid = 0;
    int32_t uuidType = -1;
    double fps;
    uint32_t drawCalls = 0;
    uint32_t entities = 0;
    uint32_t pointAmount = 0;
    uint32_t linesAmount = 0;
    uint32_t polyAmount = 0;

    void reset() {
      uuid = 0;
      uuidType = -1;
    }

  } m_state;

  void initOpenGL(int major, int minor);
  void init(Math::Vector<2, uint32_t> windowSize);

  // Variables
public:
protected:
  std::ofstream m_log;
  GLFWwindow *m_window;
  Engine *m_engine;
  std::chrono::time_point<Clock> m_lastTime;
  std::chrono::time_point<Clock> m_startTime;
};

} // namespace Engine

#endif
