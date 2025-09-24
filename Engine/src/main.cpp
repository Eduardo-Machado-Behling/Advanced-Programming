#include "GLFW/glfw3.h"
#include "engine.hpp"
#include "window.hpp"
#include <functional>
#include <iostream>
#include <vector>

struct MyWindow : public Engine::Window {
  MyWindow()
      : p0(m_engine->createPoint({100, 100}, {1, 0, 0}, 20)),
        p1(m_engine->createPoint({200, 200}, {1, 0, 0}, 20)) {
    std::cout << "[Client App] Created window" << std::endl;

    std::vector<Engine::Math::Vector<2>> verts;

    m_engine->createLine({250, 250}, {300, 300}, {1, 1, 0}, 10);

    verts.push_back({100, 100});
    verts.push_back({200, 100});
    verts.push_back({200, 200});
    m_engine->createPoly(verts, {0, 1, 0});

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, &MyWindow::StaticKeyCallback);
    glfwSetMouseButtonCallback(m_window, &MyWindow::StaticMouseCallback);
  }

  void update(double dt) override {
    Engine::Math::Vector<2> vel = {100.0, 50.0};
    // p0.setPos(p0.getPos() + (vel * (float)dt));
  }

private:
  Engine::Point p0;
  Engine::Point p1;

  void mouseCallback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      // 1. Get mouse position in screen coordinates
      double xpos, ypos;
      glfwGetCursorPos(m_window, &xpos, &ypos);

      // 2. Get window size to invert the Y-axis
      int width, height;
      glfwGetWindowSize(m_window, &width, &height);

      // 3. Convert to framebuffer coordinates (Y-inversion)
      int fb_x = static_cast<int>(xpos);
      int fb_y = height - static_cast<int>(ypos);

      // 4. Call a helper function to perform the pixel read
      std::cout << "UUID: " << m_engine->lookupObjectUUID(fb_x, fb_y) << '\n';
    }
  }

  Engine::Math::Vector<2> lineStart;
  std::vector<Engine::Math::Vector<2>> polyVerts;
  bool captureVerts = false;
  ;

  void keyCallback(int key, int scancode, int action, int mode) {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);

    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    Engine::Math::Vector<2> pos{(float)x, height - (float)y};

    if (action == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_Q:
        m_engine->createPoint(pos, {1, 0, 0}, 8);
        break;
      case GLFW_KEY_W:
        lineStart = pos;
        break;
      case GLFW_KEY_E:
        if (captureVerts) {
          polyVerts.push_back(pos);
        }
        break;

      case GLFW_KEY_LEFT_SHIFT:
      case GLFW_KEY_RIGHT_SHIFT:
        captureVerts = true;
        break;

      default:
        break;
      }
    } else if (action == GLFW_RELEASE) {

      switch (key) {
      case GLFW_KEY_Q:
        break;
      case GLFW_KEY_W:
        m_engine->createLine(lineStart, pos, {1, 1, 0}, 10);
        break;
      case GLFW_KEY_E:
        break;

      case GLFW_KEY_LEFT_SHIFT:
      case GLFW_KEY_RIGHT_SHIFT:
        captureVerts = false;
        if (!polyVerts.empty()) {
          m_engine->createPoly(polyVerts, {1, 1, 1});
          polyVerts.clear();
        }
        break;

      default:
        break;
      }
    }
  }

  static void StaticKeyCallback(GLFWwindow *win, int key, int scancode,
                                int action, int mode) {
    // Retrieve the MyWindow instance that we stored
    MyWindow *self = static_cast<MyWindow *>(glfwGetWindowUserPointer(win));

    // If the pointer is valid, call the actual member function
    if (self) {
      self->keyCallback(key, scancode, action, mode);
    }
  }

  static void StaticMouseCallback(GLFWwindow *win, int button, int action,
                                  int mode) {
    // Retrieve the MyWindow instance that we stored
    MyWindow *self = static_cast<MyWindow *>(glfwGetWindowUserPointer(win));

    // If the pointer is valid, call the actual member function
    if (self) {
      self->mouseCallback(button, action, mode);
    }
  }
};

int main() {
  MyWindow win;
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
