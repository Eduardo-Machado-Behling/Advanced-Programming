#include "Path/HexagonalGridAdapter.hpp"

#include "Grid/Manager.hpp"

std::vector<Vec2u> HexagonalGridAdapter::getNeighbors(Vec2u pos) {
  std::vector<Vec2u> neighbors;

  int col = static_cast<int>(pos[0]);
  int row = static_cast<int>(pos[1]);
  int parity = row & 1;

  static const Vec2i evenDirections[] = {{-1, -1}, {0, -1}, {1, 0},
                                         {0, 1},   {-1, 1}, {-1, 0}};
  static const Vec2i oddDirections[] = {{0, -1}, {1, -1}, {1, 0},
                                        {1, 1},  {0, 1},  {-1, 0}};

  const Vec2i *currentDir = (parity == 1) ? oddDirections : evenDirections;

  for (int i = 0; i < 6; ++i) {
    int nextCol = col + currentDir[i][0];
    int nextRow = row + currentDir[i][1];

    Vec2u next = {static_cast<uint32_t>(nextCol),
                  static_cast<uint32_t>(nextRow)};

    if (isValid(next)) {
      neighbors.push_back(next);
    }
  }
  return neighbors;
}

bool HexagonalGridAdapter::isValid(Vec2u coord) {
  GridManager &gridManager = GridManager::get();

  if (coord[1] >= gridManager.rows() || coord[0] >= gridManager.cols())
    return false;

  return true;
}