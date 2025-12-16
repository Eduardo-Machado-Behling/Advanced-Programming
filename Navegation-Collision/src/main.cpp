#include <GLFW/glfw3.h>
#include <chrono>

#include "Math/Vector.hpp"
#include "engine.hpp"
#include "window.hpp"

#include "Actions/Invoker.hpp"

#include "Cells/Factory.hpp"

#include "Grid/Manager.hpp"

#include "Actions/AddCell.hpp"
#include "Actions/RemoveCell.hpp"

#include "Chain/GridInit/Chain.hpp"
#include "Chain/GridInit/Config.hpp"
#include "Chain/GridInit/Factory.hpp"
#include "Chain/GridInit/Manager.hpp"

#include "Observer/GridChanged.hpp"

#include "Animation/Manager.hpp"

#include "Path/Manager.hpp"

struct MyWindow : public Engine::Window {
  MyWindow(int argc, const char **argv) {
    glfwSwapInterval(1);

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

#ifdef _DEBUG
    bool ask = false;
#else
    bool ask = true;
#endif

    initChain.setNext(new ::Chain::GridConfig(ask))
        ->setNext(new ::Chain::GridFactory(ask))
        ->setNext(new ::Chain::GridManager());

    displayRefreshSubscriber.setOnChange([this]() {
      refresh = true;
      // m_tickTimeStamp = std::nullopt;
    });

    GridManager::get()
        .subscribeOnCellChange(&displayRefreshSubscriber)
        .subscribeOnGridChange(&displayRefreshSubscriber);

    // anim.registerOnStateChanged(&animationState);
    anim.setPlayFunction([this](double dt) {
      if (dt == 0) {
        GridManager::get().setup();
        return false;
      }
      return GridManager::get().update(*m_engine, dt);
    });

    anim.setIdleFunction(
        [this]() { return GridManager::get().update(*m_engine); });

    animationState.setOnChange([this]() {
      switch (anim.getState()) {
      case decltype(anim)::PlayerState::INIT:
        break;

      case decltype(anim)::PlayerState::PLAYING:
        std::cout << PathManager::get().getCollisions();
        break;

      case decltype(anim)::PlayerState::PAUSED:
        break;

      case decltype(anim)::PlayerState::ENDED:
        // restore = std::numeric_limits<size_t>::max();
        GridManager::get().setup();
        refresh = true;
        break;

      default:
        break;
      }
    });
    anim.registerOnStateChanged(&animationState);
  }

  Subscribers::CallbackSubscriber displayRefreshSubscriber;
  Subscribers::CallbackSubscriber animationState;

  AnimationManager<std::chrono::high_resolution_clock> anim;
  Chain::GridAllocatorChain initChain;
  FILE *out = stdout;
  bool refresh = false;
  int restore = 0;
  std::optional<Clock::time_point> m_tickTimeStamp;
  std::vector<Vec2> m_collisionPoints;

  void update(double dt) override {
    m_state.get("sumTime") = (double)0;
    Invoker::get().execute();

    if (restore) {
      Invoker::get().restore(restore);
      restore = 0;
    }

    if (anim.getState() == decltype(anim)::PlayerState::PLAYING) {
      refresh = true;
    }

    if (refresh) {
      refresh = false;
      clearEngine();

      if (GridManager::get().allocated()) {
        Vec2 start = GridManager::get().start();
        Vec2 end = GridManager::get().end();

        m_engine->createPoint(end, {1, 1, 0}, 8);
        m_engine->createPoint(start, {0, 1, 1}, 8);
        for (auto coll : m_collisionPoints)
          m_engine->createPoint(coll, {1, 1, 0}, 8);

        anim.loop();
      }
    }
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
        m_enableTick = !m_enableTick;
        if (anim.getState() == decltype(anim)::PlayerState::PLAYING)
          anim.pause();
        else
          anim.start();
        break;

      case GLFW_KEY_F1:
        if (!m_startGrid && GridManager::get().allocated()) {
          Vec2u pos = GridManager::get().getCoord(Vec2{glx, gly});
          if (!GridManager::get().cget(pos)->empty()) {
            break;
          }
          m_startGrid = pos;

          Invoker::get().addCommand(new Commands::AddCell(
              CellFactory::get().Create(CellFactory::CellType::ORIGIN, gridPos),
              gridPos));
        }
        break;

      case GLFW_KEY_F2:
        grid->clear();
        refresh = true;
        break;

      case GLFW_KEY_F3: {
        GridManager::get().setup();
        auto colls = PathManager::get().getCollisions();
        std::cout << colls;

        for (auto &coll : colls) {
          m_collisionPoints.push_back(coll.agents[0].pos);
        }
        refresh = true;
      }

      break;

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
