#ifndef HEXAGONAL_GRID_ADAPTER_HPP
#define HEXAGONAL_GRID_ADAPTER_HPP

#include "Grid/Graph.hpp"

class HexagonalGridAdapter : public Grid::IGraph {
  std::vector<Vec2u> getNeighbors(Vec2u u) override;
  bool isValid(Vec2u coord) override;
};

#endif // HEXAGONAL_GRID_ADAPTER_HPP