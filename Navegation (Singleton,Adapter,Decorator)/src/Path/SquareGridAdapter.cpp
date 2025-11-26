#include "Path/SquareGridAdapter.hpp"

#include "Grid/Manager.hpp"

std::vector<Vec2u> SquareGridAdapter::getNeighbors(Vec2u pos) {
  std::vector<Vec2u> neighbors;
  Vec2i directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  Vec2i iPos = {(int)pos[0], (int)pos[1]};

  for (int i = 0; i < 4; ++i) {
    Vec2i iNext = iPos + directions[i];
    Vec2u next = {(uint32_t)iNext[0], (uint32_t)iNext[1]};
    if (isValid(next))
      neighbors.push_back(next);
  }
  return neighbors;
}

bool SquareGridAdapter::isValid(Vec2u coord) {
  GridManager &gridManager = GridManager::get();
  if (coord[1] >= gridManager.rows() || coord[0] >= gridManager.cols())
    return false;
  return true;
}
