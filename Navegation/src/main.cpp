#include "GLFW/glfw3.h"
#include "Math/Vector.hpp"
#include "engine.hpp"
#include "window.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <unordered_map>
#include <vector>

using Color = Engine::Math::Vector<3>;
using Cell = Engine::Math::Vector<2, uint32_t>;
using Vec2 = Engine::Math::Vector<2>;
using Vec2U = Engine::Math::Vector<2, uint32_t>;

namespace CellTypes {
struct Empty {};

struct Start {
  Cell end;

  Start() {}
  Start(Cell end) : end(end) {}
};

struct End {};

struct Obs {};

using CellType = std::variant<Empty, Start, End, Obs>;
} // namespace CellTypes

class Grid {
public:
  struct Cell {
    size_t UUID;
    CellTypes::CellType type;
  };

  void allocate(size_t rows, size_t cols, Vec2 start, Vec2 end) {
    gridSize[0] = rows;
    gridSize[1] = cols;

    mat = new Cell[rows * cols];

    box[0] = start;
    box[1] = end;
  }

  void deallocate() {
    delete[] mat;
    mat = nullptr;
    uuidToCell.clear();
  }

  const Cell *get(size_t row, size_t col) {
    return mat + gridSize[1] * row + col;
  }

  Vec2U getCoord(Vec2 mousePos) {
    mousePos -= box[0];
    Vec2 delta = box[1] - box[0];
    delta[0] /= gridSize[0];
    delta[1] /= gridSize[1];

    return {static_cast<unsigned int>((mousePos[0] / delta[0])),
            static_cast<unsigned int>((mousePos[1] / delta[1]))};
  }

  Cell *get(Vec2 mousePos) {
    Vec2U gridPos = getCoord(mousePos);
    return mat + gridPos[1] * gridSize[1] + gridPos[0];
  }

  Cell *get(size_t uuid) {
    auto coord = uuidToCell[uuid];
    return mat + gridSize[1] * coord[0] + coord[1];
  }

  void set(size_t row, size_t col, Cell type) {
    mat[gridSize[1] * row + col] = type;
    uuidToCell.insert({type.UUID, {(uint32_t)row, (uint32_t)col}});
  }

  size_t rows() { return gridSize[0]; }
  size_t cols() { return gridSize[1]; }

  Vec2 start() { return box[0]; }
  Vec2 end() { return box[1]; }

  bool allocated() { return mat; }

private:
  Cell *mat = nullptr;
  std::unordered_map<size_t, Vec2U> uuidToCell;
  Vec2U gridSize;
  Vec2 box[2];
};

struct MyWindow : public Engine::Window {
  MyWindow() {
    m_state.addHeader("objPoints");
    m_state.addHeader("robotPoints");
    m_state.addHeader("sumTime");

    m_state.get("objPoints") = 0u;
    m_state.get("robotPoints") = 0u;
    m_state.get("sumTime") = (double)0;
  }

  const float POINT_RADIUS = 12;
  const float LINE_STOKE = 2;

  const Color START_COLOR = {0, 1, 0};
  const Color END_COLOR = {1, 0, 0};
  const Color OBS_COLOR = {0.2, 0.2, 0.2};
  const Color LINE_COLOR = {1, 1, 1};

  Grid grid;

  bool refresh = true;
  void update(double dt) override {
    m_state.get("sumTime") = (double)0;

    if (refresh) {
      clearEngine();

      if (grid.allocated()) {
        Vec2 start = grid.start();
        Vec2 delta = grid.end() - start;

        m_points.emplace_back(
            m_engine->createPoint(grid.start(), {1, 1, 0}, 8));
        m_points.emplace_back(m_engine->createPoint(grid.end(), {0, 1, 1}, 8));

        float xstep = delta[0] / grid.rows();
        float ystep = delta[1] / grid.cols();

        for (size_t i = 0; i < grid.rows(); i++) {
          for (size_t j = 0; j < grid.cols(); j++) {
            size_t nj = (j + 1) % grid.cols();

            Vec2 pos0 = {xstep * j, ystep * i};
            pos0 += start;

            Vec2 corner[4] = {pos0, pos0, pos0, pos0};

            corner[1][0] += xstep;

            corner[2][1] += ystep;

            corner[3][0] += xstep;
            corner[3][1] += ystep;

            m_lines.emplace_back(m_engine->createLine(corner[0], corner[1],
                                                      LINE_COLOR, LINE_STOKE));
            m_lines.emplace_back(m_engine->createLine(corner[0], corner[2],
                                                      LINE_COLOR, LINE_STOKE));
            m_lines.emplace_back(m_engine->createLine(corner[1], corner[3],
                                                      LINE_COLOR, LINE_STOKE));
            m_lines.emplace_back(m_engine->createLine(corner[2], corner[3],
                                                      LINE_COLOR, LINE_STOKE));

            std::vector<Vec2> vertices = {corner[0], corner[1], corner[2],
                                          corner[3]};

            std::visit(
                [this, pos0, xstep, ystep, &vertices, i, j](auto &&arg) {
                  using T = std::decay_t<decltype(arg)>;
                  if constexpr (std::is_same_v<T, CellTypes::Start>) {
                    std::cout << "x=" << j << ", y= " << i << "[" << i << ", "
                              << j << "]: ";
                    std::cout << "start -> " << arg.end[0] << ", " << arg.end[1]
                              << '\n';
                    m_points.emplace_back(
                        m_engine->createPoint(pos0 + Vec2{xstep / 2, ystep / 2},
                                              START_COLOR, POINT_RADIUS));
                  }

                  else if constexpr (std::is_same_v<T, CellTypes::End>) {
                    std::cout << i << ", " << j << ": ";
                    std::cout << "end\n";

                    m_points.emplace_back(
                        m_engine->createPoint(pos0 + Vec2{xstep / 2, ystep / 2},
                                              END_COLOR, POINT_RADIUS));
                  } else if constexpr (std::is_same_v<T, CellTypes::Obs>) {
                    std::cout << i << ", " << j << ": ";
                    std::cout << "obs\n";

                    m_polys.emplace_back(m_engine->createPoly(
                        vertices, OBS_COLOR, OBS_COLOR, 0.0, false));

                  } else if constexpr (std::is_same_v<T, CellTypes::Empty>) {
                  } else
                    static_assert(false, "non-exhaustive visitor!");
                },
                grid.get(i, j)->type);
          }
        }
      }
    }

    refresh = false;
  }

private:
  Vec2 start;
  Vec2 end;
  int mouseHolding = 0;

  void mouseButtonCallback(int button, int action, int mods) override {
    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    if (action == GLFW_PRESS) {
      auto uid = m_engine->lookupObjectUUID(glx, gly);
      int type = m_engine->getType(uid);

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!grid.allocated()) {
          start = {glx, gly};
        } else {
          mouseHolding = button;
        }
      }
    } else if (action == GLFW_RELEASE) {
      mouseHolding = 0;
      if (button == GLFW_MOUSE_BUTTON_LEFT) {

        if (!grid.allocated()) {
          end = {glx, gly};

          size_t rows = 10, cols = 10;

          // std::cout << "Rows : ";
          // std::cin >> rows;
          // std::cout << "cols : ";
          // std::cin >> cols;

          grid.allocate(rows, cols, start, end);

          refresh = true;
        }       }
    }
  }

  void clear() { grid.deallocate(); }

  std::optional<Vec2> startUidTemp = std::nullopt;

  void keyCallback(int key, int scancode, int action, int mode) override {
    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    auto uid = m_engine->lookupObjectUUID(glx, gly);

    if (action == GLFW_PRESS) {
      bool num = false;
      switch (key) {
      case GLFW_KEY_Q:
        clear();
        break;

      case GLFW_KEY_ESCAPE:
        terminate();
        break;

      case GLFW_KEY_F1:
        if (!startUidTemp) {
          startUidTemp = Vec2({glx, gly});

          grid.get(*startUidTemp)->type = CellTypes::Start();

          refresh = true;
        }
        break;

      case GLFW_KEY_F2:
        grid.get({glx, gly})->type = CellTypes::Obs();
        refresh = true;
        break;

      case GLFW_KEY_LEFT_SHIFT:
        break;

      default:
        break;
      }

    } else if (action == GLFW_RELEASE) {
      switch (key) {
      case GLFW_KEY_F1:
        if (startUidTemp) {
          Vec2 end = {glx, gly};
          Vec2U endGrid = grid.getCoord(end);
          Vec2U startGrid = grid.getCoord(*startUidTemp);

          std::cout << "F1 R:" << startGrid[0] << ", " << startGrid[1] << " -> "
                    << endGrid[0] << ", " << endGrid[1] << '\n';

          if (endGrid == startGrid) {
            grid.get(*startUidTemp)->type = CellTypes::Empty();
            std::cout << "short\n";
          } else {
            std::get<CellTypes::Start>(grid.get(*startUidTemp)->type).end =
                endGrid;
            grid.get(end)->type = CellTypes::End();
          }

          startUidTemp = std::nullopt;
          refresh = true;
        }
        break;

      default:
        break;
      }
    }
  }

  void cursorPosCallback(double xpos, double ypos) override {
    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    if (mouseHolding == GLFW_MOUSE_BUTTON_LEFT) {
      grid.get({glx, gly})->type = CellTypes::Obs();
      refresh = true;
    } else if (mouseHolding == GLFW_MOUSE_BUTTON_RIGHT) {
      grid.get({glx, gly})->type = CellTypes::Empty();
      refresh = true;
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
