#include "GLFW/glfw3.h"
#include "Math/Vector.hpp"
#include "engine.hpp"
#include "window.hpp"

#include "Cells/Factory.hpp"

#include "Grid/Factories/Hexagonal.hpp"
#include "Grid/Factories/Square.hpp"
#include "Grid/Manager.hpp"

struct MyWindow : public Engine::Window {
  struct GridArgs {
    int gridWidth = 10;
    int gridHeight = 10;

    float width = 0;
    float height = 0;
    float posX = 0;
    float posY = 0;

    bool gen = false;
    int startCount = 0;
    int obsCount = 0;
    float minDist = 1;
    int runAmount = 32;
    int sampleAmount = 50;
  } config;

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

    std::unordered_map<std::string, std::string> args;

    std::string specifier;
    for (size_t i = 1; i < argc; i++) {
      std::string arg = argv[i];

      if (arg[0] == '-') {
        specifier = arg.substr(1);
      } else if (!specifier.empty()) {
        args.emplace(specifier, arg);
      }
    }

    for (auto &[var, val] : args) {
      if (var == "w") {
        config.width = std::stof(val);
      } else if (var == "h") {
        config.height = std::stof(val);
      } else if (var == "x") {
        config.posX = std::stof(val);
      } else if (var == "y") {
        config.posY = std::stof(val);
      } else if (var == "gw") {
        config.gridWidth = std::stoi(val);
      } else if (var == "gh") {
        config.gridHeight = std::stoi(val);
      } else if (var == "sc") {
        config.startCount = std::stoi(val);
        config.gen = true;
      } else if (var == "oc") {
        config.obsCount = std::stoi(val);
      } else if (var == "md") {
        config.minDist = std::stof(val);
      } else if (var == "ra") {
        config.runAmount = std::stoi(val);
      } else if (var == "sa") {
        config.sampleAmount = std::stoi(val);
      }
    }

    if (config.width && config.height) {
      GridManager::get().allocate(
          config.gridHeight, config.gridWidth, {config.posX, config.posY},
          {config.posX + config.width, config.posY - config.height});
    }
  }

  FILE *out = stdout;
  bool refresh = true;
  std::optional<Clock::time_point> m_tickTimeStamp;

  void update(double dt) override {
    m_state.get("sumTime") = (double)0;

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

          size_t rows = 10, cols = 10;

          std::cout << "Rows : ";
          std::cin >> rows;
          std::cout << "cols : ";
          std::cin >> cols;

          int type;
          std::cout << "type (1=Square, 2=Hex): ";
          std::cin >> type;

          GridFactories::IFactory *factory = nullptr;
          if (type == 2) {
            factory = new GridFactories::HexagonalFactory();
          } else {
            factory = new GridFactories::SquareFactory();
          }
          GridManager::get().allocate(rows, cols, start, end, factory);

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

  std::optional<Vec2u> m_startGrid = std::nullopt;
  bool m_enableTick = false;

  void keyCallback(int key, int scancode, int action, int mode) override {
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
        if (!m_startGrid) {
          m_startGrid = GridManager::get().getCoord(Vec2{glx, gly});

          grid->set(CellFactory::get().Create(CellFactory::CellType::ORIGIN,
                                              gridPos));

          refresh = true;
        }
        break;

      case GLFW_KEY_F2:
        grid->clear();
        refresh = true;
        break;

      case GLFW_KEY_F3:
        std::cout << "Path amount to generate: ";
        std::cin >> config.startCount;
        config.obsCount = 0;
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

          if (endGrid == *m_startGrid) {
            GridManager::get().get(*m_startGrid)->clear();
            std::cout << "short\n";
          } else {
            GridManager::get()
                .get(*m_startGrid)
                ->set(CellFactory::get().Create(CellFactory::CellType::ORIGIN,
                                                *m_startGrid, endGrid));
            GridManager::get().get(endGrid)->set(CellFactory::get().Create(
                CellFactory::CellType::DESTINATION, endGrid, *m_startGrid));
          }

          m_startGrid = std::nullopt;
          refresh = true;
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
    Grid::IGrid *grid = GridManager::get().get(gridPos);

    if (!grid)
      return;

    if (mouseHolding != -1) {
      grid->clear();
      if (mouseHolding == GLFW_MOUSE_BUTTON_LEFT) {
        grid->set(
            CellFactory::get().Create(CellFactory::CellType::OBS, gridPos));
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
