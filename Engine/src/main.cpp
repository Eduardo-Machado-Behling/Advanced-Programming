#include "GLFW/glfw3.h"
#include "window.hpp"
#include <functional>
#include <iostream>

struct MyWindow : public Engine::Window {
  MyWindow() {
    std::cout << "[Client App] Created window" << std::endl;
    m_engine->createPoint({100, 100}, {1, 0, 0}, 20);
    m_engine->createPoint({200, 200}, {1, 0, 0}, 20);

    std::vector<Engine::Math::Vector<2>> verts;

    m_engine->createLine({250, 250}, {300, 300}, {1, 1, 0}, 10);

    verts.push_back({100, 100});
    verts.push_back({200, 100});
    verts.push_back({200, 200});
    m_engine->createPoly(verts, {0, 1, 0});

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, &MyWindow::StaticKeyCallback);
  }

  void update(double dt) override {}

private:
  void keyCallback(int key, int scancode, int action, int mode) {
	  if(action == GLFW_PRESS){
		double x, y;
		glfwGetCursorPos(m_window, &x, &y);

		switch (key) {
			case GLFW_KEY_Q:
				m_engine->createPoint({(float)x, m_engine->winSize()[1] - (float)y}, {1,0,0}, 2);
				break;
			case GLFW_KEY_W:
				break;
			case GLFW_KEY_E:
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
};

int main() {
  MyWindow win;
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
