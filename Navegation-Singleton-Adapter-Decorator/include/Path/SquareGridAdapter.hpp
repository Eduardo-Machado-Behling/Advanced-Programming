#ifndef SQUARE_GRID_ADAPTER_HPP
#define SQUARE_GRID_ADAPTER_HPP

#include "Grid/Graph.hpp"

class SquareGridAdapter : public Grid::IGraph {
  std::vector<Vec2u> getNeighbors(Vec2u u) override;
  bool isValid(Vec2u coord) override;
};

#endif // SQUARE_GRID_ADAPTER_HPP