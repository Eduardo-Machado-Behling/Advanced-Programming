#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <chrono>

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

  virtual void update(double dt) = 0;

  // Variables
public:
protected:
  GLFWwindow *m_window;
  Engine *m_engine;
  std::chrono::time_point<Clock> m_lastTime;
};

} // namespace Engine

#endif
