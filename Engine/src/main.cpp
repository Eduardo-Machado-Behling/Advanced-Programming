#include "GLFW/glfw3.h"
#include "Wrappers/Line.hpp"
#include "Wrappers/Point.hpp"
#include "Wrappers/Poly.hpp"
#include "engine.hpp"
#include "window.hpp"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

struct MyWindow : public Engine::Window {
  MyWindow()
      : p0(m_engine->createPoint({100, 100}, {1, 0, 0}, 20)),
        p1(m_engine->createPoint({200, 200}, {1, 0, 0}, 20)),
        l(std::move(
            m_engine->createLine({250, 250}, {300, 300}, {1, 1, 0}, 10))) {
    std::cout << "[Client App] Created window" << std::endl;

    std::vector<Engine::Math::Vector<2>> verts;

    verts.push_back({100, 100});
    verts.push_back({200, 100});
    verts.push_back({200, 200});
    p.reset(
        new Engine::Poly(std::move(m_engine->createPoly(verts, {0, 1, 0}))));
  }

  Engine::Math::Vector<2> vel = {-100.0, 50.0};
  float period = 1.000;
  float width = 100;
  double count = 0;

  void update(double dt) override {
    auto newPos = p0.getPos() + (vel * (float)dt);
    auto winSize = m_engine->winSize();

    // M_PI * 2 -> 0.050
    // x -> y

    float angle = (count * M_PI * 2) / period;
    Engine::Math::Vector<2> dir = {cosf(angle), sinf(angle)};
    auto pos0 = std::get<0>(l.getVerts());
    auto pos1 = pos0 + (dir * width);
    count += dt;
    if (count > period) {
      count -= period;
    }

    l.setVerts(pos0, pos1);

    if (newPos[0] > winSize[0] || newPos[0] < 0) {
      vel[0] *= -1;
    }

    if (newPos[1] > winSize[1] || newPos[1] < 0) {
      vel[1] *= -1;
    }

    newPos = p0.getPos() + (vel * (float)dt);
    p0.setPos(newPos);
  }

private:
  Engine::Point p0;
  Engine::Point p1;
  Engine::Line l;
  std::unique_ptr<Engine::Poly> p;

  void mouseButtonCallback(int button, int action, int mods) override {
    if (action == GLFW_PRESS) {
      double xpos, ypos;
      glfwGetCursorPos(m_window, &xpos, &ypos);

      int width, height;
      glfwGetWindowSize(m_window, &width, &height);

      int fb_x = static_cast<int>(xpos);
      int fb_y = height - static_cast<int>(ypos);

      uint32_t uuid = m_engine->lookupObjectUUID(fb_x, fb_y);
      if (uuid == 0) {
        return;
      }

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        std::cout << "Pressed: " << uuid << '\n';
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        m_engine->remove(uuid);
      }
    }
  }

  Engine::Math::Vector<2> lineStart;
  std::vector<Engine::Math::Vector<2>> polyVerts;
  bool captureVerts = false;

  void keyCallback(int key, int scancode, int action, int mode) override {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);

    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    Engine::Math::Vector<2> pos{(float)x, height - (float)y};

    if (action == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_Q:
        m_engine->createPoint(pos, {1, 0, 0}, 24);
        break;
      case GLFW_KEY_W:
        lineStart = pos;
        break;
      case GLFW_KEY_E:
        if (captureVerts) {
          m_engine->createPoint(pos, {1, 0, 0}, 8);
          p->addVert(pos);
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
        break;

      default:
        break;
      }
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
