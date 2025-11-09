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

struct End {
  Cell start;

  End(Cell start) : start(start) {}
};

struct Obs {};

using CellType = std::variant<Empty, Start, End, Obs>;
} // namespace CellTypes

class Grid {
public:
  struct Cell {
    size_t UUID;
    CellTypes::CellType type;
  };

  void clear() {
    for (size_t i = 0; i < rows(); i++) {
      for (size_t j = 0; j < cols(); j++) {
        _get(i, j)->type = CellTypes::Empty();
      }
    }
  }

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

  const Cell *get(size_t row, size_t col) const {
    return mat + gridSize[1] * row + col;
  }

  Cell *_get(size_t row, size_t col) const {
    return mat + gridSize[1] * row + col;
  }

  Vec2U getCoord(Vec2 mousePos) {
    mousePos -= box[0];
    Vec2 delta = box[1] - box[0];
    delta[0] /= gridSize[1];
    delta[1] /= gridSize[0];

    return {static_cast<unsigned int>((mousePos[0] / delta[0])),
            static_cast<unsigned int>((mousePos[1] / delta[1]))};
  }

  Cell *get(Vec2 mousePos) {
    Vec2U gridPos = getCoord(mousePos);

    if (gridPos[0] >= gridSize[1] || gridPos[1] >= gridSize[0])
      return nullptr;
    return mat + gridPos[1] * gridSize[1] + gridPos[0];
  }

  Cell *get(Vec2U gridPos) {
    if (gridPos[0] >= gridSize[1] || gridPos[1] >= gridSize[0])
      return nullptr;

    return mat + gridPos[1] * gridSize[1] + gridPos[0];
  }

  const Cell *get(Vec2U gridPos) const {
    if (gridPos[0] >= gridSize[1] || gridPos[1] >= gridSize[0])
      return nullptr;

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

  void dump(FILE *out) {
    for (size_t i = 0; i < rows(); i++) {
      fputc('|', out);
      for (size_t j = 0; j < cols(); j++) {
        std::visit(
            [out](auto &&arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, CellTypes::Start>) {
                fputc('S', out);
              }

              else if constexpr (std::is_same_v<T, CellTypes::End>) {
                fputc('E', out);
              } else if constexpr (std::is_same_v<T, CellTypes::Obs>) {
                fputc('O', out);
              } else if constexpr (std::is_same_v<T, CellTypes::Empty>) {
                fputc(' ', out);
              } else
                static_assert(false, "non-exhaustive visitor!");
            },
            get(i, j)->type);
        fputc('|', out);
      }
      fputc('\n', out);
    }
    fputc('\n', out);
  }

  size_t rows() const { return gridSize[0]; }
  size_t cols() const { return gridSize[1]; }

  Vec2 start() const { return box[0]; }
  Vec2 end() const { return box[1]; }

  bool allocated() const { return mat; }

private:
  Cell *mat = nullptr;
  std::unordered_map<size_t, Vec2U> uuidToCell;
  Vec2U gridSize;
  Vec2 box[2];
};

class Dijkstra {
  struct Node {
    Vec2U pos;
    int dist;

    Node(int dist, Vec2U pos) : dist(dist), pos(pos) {}

    bool operator>(const Node &other) const { return dist > other.dist; }
  };

public:
  static std::vector<Vec2U> ShortestPath(const Grid &grid, Vec2U start,
                                         Vec2U end) {
    if (!grid.allocated())
      return {};

    if (!std::holds_alternative<CellTypes::Start>(grid.get(start)->type) ||
        !std::holds_alternative<CellTypes::End>(grid.get(end)->type))
      return {};

    const uint32_t UINTMAX = std::numeric_limits<uint32_t>::max();

    std::vector<std::vector<int>> dist(
        grid.rows(),
        std::vector<int>(grid.cols(), std::numeric_limits<int>::max()));
    std::vector<std::vector<Vec2U>> parent(
        grid.rows(), std::vector<Vec2U>(grid.cols(), {UINTMAX, UINTMAX}));

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    dist[start[1]][start[0]] = 0;
    pq.push({0, start});

    int dr[] = {-1, 1, 0, 0}; // Up, Down
    int dc[] = {0, 0, -1, 1}; // Left, Right

    while (!pq.empty()) {
      Node current = pq.top();
      pq.pop();

      int d = current.dist;
      Vec2U u = current.pos;

      if (u == end) {
        break;
      }

      if (d > dist[u[1]][u[0]]) {
        continue;
      }

      for (int i = 0; i < 4; ++i) {
        uint32_t nr = u[1] + dr[i];
        uint32_t nc = u[0] + dc[i];

        if (nr >= 0 && nr < grid.rows() && nc >= 0 && nc < grid.cols() &&
            !std::holds_alternative<CellTypes::Obs>(grid.get(nr, nc)->type)) {

          int newDist = dist[u[1]][u[0]] + 1;

          if (newDist < dist[nr][nc]) {
            dist[nr][nc] = newDist;

            parent[nr][nc] = u;

            pq.push({newDist, {nc, nr}});
          }
        }
      }
    }

    std::vector<Vec2U> path;

    if (dist[end[1]][end[0]] == std::numeric_limits<int>::max()) {
      return path; // Return empty path
    }

    Vec2U curr = end;
    while (!(curr[1] == UINTMAX && curr[0] == UINTMAX)) {
      path.push_back(curr);
      curr = parent[curr[1]][curr[0]];
    }

    reverse(path.begin(), path.end());

    return path;
  }
};

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
      grid.allocate(config.gridHeight, config.gridWidth,
                    {config.posX, config.posY},
                    {config.posX + config.width, config.posY - config.height});
    }

    if (config.gen && grid.allocated()) {
      out = fopen("boards.log", "w");
      for (uint32_t i = 1; i <= config.sampleAmount; i++) {
        m_state.get("sample") = i;
        grid.clear();
        generateBoard();
        for (uint32_t _ = 1; _ <= config.runAmount; _++) {
          m_state.get("run") = _;
          run();
        }
      }

      fclose(out);
      terminate();
    }
  }

  FILE *out = stdout;
  const float POINT_RADIUS = 12;
  const float LINE_STOKE = 2;

  const Color START_COLOR = {0, 1, 0};
  const Color END_COLOR = {1, 0, 0};
  const Color OBS_COLOR = {0.2, 0.2, 0.2};
  const Color LINE_COLOR = {1, 1, 1};

  Grid grid;

  bool refresh = true;
  double timer = 0;
  const double wait = 0.1;
  size_t animI = 0;

  void update(double dt) override {
    m_state.get("sumTime") = (double)0;

    if (!paths.empty())
      timer += dt;
    else
      timer = 0;

    if (timer >= wait) {
      refresh = true;
      animI++;
      timer -= wait;
    }

    if (refresh) {
      clearEngine();

      if (grid.allocated()) {
        Vec2 start = grid.start();
        Vec2 delta = grid.end() - start;

        m_points.emplace_back(
            m_engine->createPoint(grid.start(), {1, 1, 0}, 8));
        m_points.emplace_back(m_engine->createPoint(grid.end(), {0, 1, 1}, 8));

        float xstep = delta[0] / grid.cols();
        float ystep = delta[1] / grid.rows();

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
                    m_points.emplace_back(
                        m_engine->createPoint(pos0 + Vec2{xstep / 2, ystep / 2},
                                              START_COLOR, POINT_RADIUS));
                  }

                  else if constexpr (std::is_same_v<T, CellTypes::End>) {
                    m_points.emplace_back(
                        m_engine->createPoint(pos0 + Vec2{xstep / 2, ystep / 2},
                                              END_COLOR, POINT_RADIUS));
                  } else if constexpr (std::is_same_v<T, CellTypes::Obs>) {
                    m_polys.emplace_back(m_engine->createPoly(
                        vertices, OBS_COLOR, OBS_COLOR, 0.0, false));

                  } else if constexpr (std::is_same_v<T, CellTypes::Empty>) {
                  } else
                    static_assert(false, "non-exhaustive visitor!");
                },
                grid.get(i, j)->type);
          }
        }

        start += (Vec2({xstep, ystep}) * 0.5f);

        bool all = true;
        for (int pathIdx = paths.size() - 1; pathIdx >= 0; pathIdx--) {
          std::vector<Vec2U> &path = paths[pathIdx];
          size_t maxI = animI < path.size() ? animI : path.size();

          if (maxI != path.size()) {
            all = false;
          }

          for (size_t ni = 1; ni < maxI; ni++) {
            size_t i = ni - 1;

            Vec2 pos0 = start + Vec2({xstep * path[i][0], ystep * path[i][1]});
            Vec2 pos1 =
                start + Vec2({xstep * path[ni][0], ystep * path[ni][1]});

            m_lines.emplace_back(
                m_engine->createLine(pos0, pos1, {0.8, 0.8, 0.2}, 2));
          }
        }

        if (all) {
          paths.clear();
          animI = 0;
        }
      }
    }

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
        if (!grid.allocated()) {
          start = {glx, gly};
        } else {
          cursorPosCallback(0, 0);
        }
      }
    } else if (action == GLFW_RELEASE) {
      mouseHolding = -1;

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!grid.allocated()) {
          end = {glx, gly};

          size_t rows = 10, cols = 10;

          std::cout << "Rows : ";
          std::cin >> rows;
          std::cout << "cols : ";
          std::cin >> cols;

          grid.allocate(rows, cols, start, end);

          refresh = true;
        } else {
          cursorPosCallback(0, 0);
        }
      }
    }
  }

  void clear() {
    grid.deallocate();
    paths.clear();
    refresh = true;
  }

  std::vector<std::vector<Vec2U>> paths;

  void path() {
    grid.dump(out);

    paths.clear();

    m_state.get("rows") = (uint32_t)grid.rows();
    m_state.get("cols") = (uint32_t)grid.cols();

    uint32_t pathAmount = 0;
    uint32_t obsAmount = 0;

    std::vector<std::pair<Vec2U, const CellTypes::Start *>> starts;
    for (uint32_t i = 0; i < grid.rows(); i++) {
      for (uint32_t j = 0; j < grid.cols(); j++) {
        const Grid::Cell *cell = grid.get(i, j);

        if (!cell) {
          continue;
        }

        if (std::holds_alternative<CellTypes::Start>(cell->type)) {
          starts.push_back({{j, i}, &std::get<CellTypes::Start>(cell->type)});
          pathAmount++;
        } else if (std::holds_alternative<CellTypes::Obs>(cell->type)) {
          obsAmount++;
        }
      }
    }

    m_state.get("pathAmount") = pathAmount;
    m_state.get("obsAmount") = obsAmount;
    for (const auto [pos, start] : starts) {
      auto begin = Clock::now();
      auto path = Dijkstra::ShortestPath(grid, pos, start->end);
      auto end = Clock::now();
      double duration =
          std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)
              .count();

      m_state.get("pathTime") = duration;
      m_state.get("pathDist") = (uint32_t)path.size();
      this->log();

      paths.push_back(path);
    }
  }

  std::optional<Vec2> startUidTemp = std::nullopt;

  void keyCallback(int key, int scancode, int action, int mode) override {
    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    auto uid = m_engine->lookupObjectUUID(glx, gly);

    Grid::Cell *cell = grid.get(Vec2{glx, gly});

    if (action == GLFW_PRESS) {
      bool num = false;
      if (!cell)
        return;

      switch (key) {
      case GLFW_KEY_Q:
        clear();
        break;

      case GLFW_KEY_ESCAPE:
        terminate();
        break;

      case GLFW_KEY_SPACE:
        path();
        break;

      case GLFW_KEY_F1:
        if (!startUidTemp) {
          startUidTemp = Vec2({glx, gly});

          grid.get(*startUidTemp)->type = CellTypes::Start();

          refresh = true;
        }
        break;

      case GLFW_KEY_F2:
        grid.get(Vec2{glx, gly})->type = CellTypes::Obs();
        refresh = true;
        break;

      case GLFW_KEY_F3:
        std::cout << "Path amount to generate: ";
        std::cin >> config.startCount;
        config.obsCount = 0;
        generateBoard();
        refresh = true;

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

          if (endGrid == startGrid) {
            grid.get(*startUidTemp)->type = CellTypes::Empty();
            std::cout << "short\n";
          } else {
            std::get<CellTypes::Start>(grid.get(*startUidTemp)->type).end =
                endGrid;
            grid.get(end)->type = CellTypes::End(startGrid);
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
    if (!grid.allocated())
      return;

    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    Grid::Cell *cell = grid.get(Vec2{glx, gly});

    if (!cell)
      return;

    if (mouseHolding == GLFW_MOUSE_BUTTON_LEFT) {
      if (std::holds_alternative<CellTypes::Empty>(cell->type)) {
        cell->type = CellTypes::Obs();
        refresh = true;
      }
    } else if (mouseHolding == GLFW_MOUSE_BUTTON_RIGHT) {
      if (std::holds_alternative<CellTypes::Start>(cell->type)) {
        grid.get(std::get<CellTypes::Start>(cell->type).end)->type =
            CellTypes::Empty();
      } else if (std::holds_alternative<CellTypes::End>(cell->type)) {
        grid.get(std::get<CellTypes::End>(cell->type).start)->type =
            CellTypes::Empty();
      }

      cell->type = CellTypes::Empty();
      refresh = true;
    }
  }

  void generateBoard() {
    std::uniform_int_distribution<uint32_t> rowGen(0, grid.rows() - 1);
    std::uniform_int_distribution<uint32_t> colGen(0, grid.cols() - 1);

    uint32_t row;
    uint32_t col;
    for (size_t i = 0; i < config.startCount; i++) {
      do {
        row = rowGen(gen);
        col = colGen(gen);
      } while (
          !std::holds_alternative<CellTypes::Empty>(grid.get(row, col)->type));

      Vec2U st({col, row});

      do {
        row = rowGen(gen);
        col = colGen(gen);
      } while (
          !std::holds_alternative<CellTypes::Empty>(grid.get(row, col)->type));

      Vec2U end({col, row});

      grid.get(st)->type = CellTypes::Start(end);
      grid.get(end)->type = CellTypes::End(st);
    }

    for (size_t i = 0; i < config.obsCount; i++) {
      do {
        row = rowGen(gen);
        col = colGen(gen);
      } while (
          !std::holds_alternative<CellTypes::Empty>(grid.get(row, col)->type));

      grid.get(Vec2U{col, row})->type = CellTypes::Obs();
    }
  }

  void run() { path(); }
};

int main(int argc, const char **argv) {
  MyWindow win(argc, argv);
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
