#include "GLFW/glfw3.h"

#include "Math/Vector.hpp"
#include "engine.hpp"
#include "window.hpp"

#include "Actions/Invoker.hpp"

#include "Cells/Factory.hpp"

#include "Grid/Manager.hpp"

#include "Actions/AddCell.hpp"
#include "Actions/RemoveCell.hpp"

#include "Chain/GridConfig.hpp"
#include "Chain/GridFactory.hpp"
#include "Chain/GridManager.hpp"

#include "Observer/GridChanged.hpp"

struct MyWindow : public Engine::Window {
  MyWindow(int argc, const char **argv) {
    m_state.addHeader("obsAmount");
    m_state.addHeader("pathAmount");
    m_state.addHeader("rows");
    m_state.addHeader("cols");
    m_state.addHeader("pathTime");
    m_state.addHeader("pathDist");
    m_state.addHeader("sample");
    m_state.addHeader("run");

    m_state.get("obsAmount") = 0u;
    m_state.get("pathAmount") = 0u;
    m_state.get("rows") = 0u;
    m_state.get("cols") = 0u;
    m_state.get("pathTime") = (double)0;
    m_state.get("pathDist") = 0u;
    m_state.get("sample") = 1;
    m_state.get("run") = 1;

    initChain.setNext(new ::Chain::GridFactory())
        ->setNext(new Chain::GridManager());

    gridSubscriber.setOnChange([this]() {
			refresh = true;
	});

	GridManager::get().subscribe(&gridSubscriber);
  }

  Subscribers::GridChanged gridSubscriber;
  Chain::GridConfig initChain;
  FILE *out = stdout;
  bool refresh = true;
  std::optional<Clock::time_point> m_tickTimeStamp;

  void update(double dt) override {
    m_state.get("sumTime") = (double)0;
    Invoker::get().execute();

    if (refresh) {
      clearEngine();

      if (GridManager::get().allocated()) {
        Vec2 start = GridManager::get().start();
        Vec2 end = GridManager::get().end();
        Vec2 delta = end - start;

        m_engine->createPoint(end, {1, 1, 0}, 8);
        m_engine->createPoint(start, {0, 1, 1}, 8);

        size_t cols = GridManager::get().cols();
        size_t rows = GridManager::get().rows();

        float xstep = delta[0] / cols;
        float ystep = delta[1] / rows;

        if (!m_enableTick) {
          GridManager::get().update(*m_engine);
        } else {
          double dt = 0;
          if (!m_tickTimeStamp) {
            m_tickTimeStamp = Clock::now();
          } else {
            dt = (Clock::now() - *m_tickTimeStamp).count();
          }

          if (GridManager::get().update(*m_engine, dt)) {
            m_enableTick = false;
            m_tickTimeStamp = std::nullopt;
          }
        }
      }
    }

    if (!m_enableTick)
      refresh = false;
  }

private:
  Vec2 start;
  Vec2 end;
  int mouseHolding = -1;

  void mouseButtonCallback(int button, int action, int mods) override {
    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    if (action == GLFW_PRESS) {
      auto uid = m_engine->lookupObjectUUID(glx, gly);
      int type = m_engine->getType(uid);

      mouseHolding = button;
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!GridManager::get().allocated()) {
          start = {glx, gly};
        } else {
          cursorPosCallback(0, 0);
        }
      }
    } else if (action == GLFW_RELEASE) {
      mouseHolding = -1;

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!GridManager::get().allocated()) {
          end = {glx, gly};
          GridManager::AllocationParam param;
          param.start = start;
          param.end = end;

          if (initChain.handle(param))
            refresh = true;
        } else {
          cursorPosCallback(0, 0);
        }
      }
    }
  }

  void clear() {
    GridManager::get().deallocate();
    refresh = true;
  }

  bool m_enableTick = false;

  void keyCallback(int key, int scancode, int action, int mode) override {
    static std::optional<Vec2u> m_startGrid = std::nullopt;
    if (!GridManager::get().allocated())
      return;

    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    auto uid = m_engine->lookupObjectUUID(glx, gly);

    Vec2u gridPos = GridManager::get().getCoord(Vec2{glx, gly});
    Grid::IGrid *grid = GridManager::get().get(gridPos);

    if (action == GLFW_PRESS) {
      if (!grid)
        return;

      switch (key) {
      case GLFW_KEY_Q:
        clear();
        break;

      case GLFW_KEY_ESCAPE:
        terminate();
        break;

      case GLFW_KEY_SPACE:
        m_enableTick = true;
        m_tickTimeStamp = std::nullopt;
        refresh = true;
        break;

      case GLFW_KEY_F1:
        if (!m_startGrid && GridManager::get().allocated()) {
          Vec2u pos = GridManager::get().getCoord(Vec2{glx, gly});
          if (!GridManager::get().cget(pos)->empty()) {
            break;
          }
          m_startGrid = pos;

          // grid->set(CellFactory::get().Create(CellFactory::CellType::ORIGIN,
          //                                     gridPos));
          Invoker::get().addCommand(new Commands::AddCell(
              CellFactory::get().Create(CellFactory::CellType::ORIGIN, gridPos),
              gridPos));

        }
        break;

      case GLFW_KEY_F2:
        grid->clear();
        refresh = true;
        break;

      case GLFW_KEY_F3:
        std::cout << "Path amount to generate: ";
        refresh = true;

      case GLFW_KEY_LEFT_SHIFT:
        break;

      default:
        break;
      }

    } else if (action == GLFW_RELEASE) {
      switch (key) {
      case GLFW_KEY_F1:
        if (m_startGrid) {
          Vec2 end = {glx, gly};
          Vec2u endGrid = GridManager::get().getCoord(end);

          if (endGrid == *m_startGrid ||
              !GridManager::get().cget(endGrid)->empty()) {
            // GridManager::get().get(*m_startGrid)->clear();
            Invoker::get().addCommand(new Commands::RemoveCell(*m_startGrid));
            std::cout << "short\n";
          } else {
            Invoker::get().addCommand(new Commands::AddCell(
                CellFactory::get().Create(CellFactory::CellType::ORIGIN,
                                          *m_startGrid, endGrid),
                *m_startGrid));

            Invoker::get().addCommand(new Commands::AddCell(
                CellFactory::get().Create(CellFactory::CellType::DESTINATION,
                                          endGrid, *m_startGrid),
                endGrid));
          }

          m_startGrid = std::nullopt;
        }
        break;

      default:
        break;
      }
    }
  }

  void cursorPosCallback(double xpos, double ypos) override {
    if (!GridManager::get().allocated())
      return;

    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    Vec2u gridPos = GridManager::get().getCoord(Vec2{glx, gly});

    if (mouseHolding != -1) {
      if (mouseHolding == GLFW_MOUSE_BUTTON_LEFT) {

        Invoker::get().addCommand(new Commands::AddCell(
            CellFactory::get().Create(CellFactory::CellType::OBS, gridPos),
            gridPos));

        // grid->set(
        //     CellFactory::get().Create(CellFactory::CellType::OBS, gridPos));
      } else if (mouseHolding == GLFW_MOUSE_BUTTON_RIGHT) {
        Invoker::get().addCommand(new Commands::RemoveCell(gridPos));
      }
      refresh = true;
    }
  }
};

int main(int argc, const char **argv) {
  MyWindow win(argc, argv);
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
