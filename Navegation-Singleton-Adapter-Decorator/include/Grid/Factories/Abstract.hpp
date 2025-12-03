#ifndef GRID_ABSTRACT_FACTORY_HPP
#define GRID_ABSTRACT_FACTORY_HPP

#include "Grid/Graph.hpp"
#include "Grid/Grid.hpp"

namespace GridFactories {
struct IFactory {
  virtual ~IFactory() = default;

  virtual Grid::IGrid *createGrid(Vec2u pos) const = 0;
  virtual Grid::IGraph *createGraph() const = 0;

  virtual Vec2 calculateDelta(Vec2 areaSize, size_t rows,
                              size_t cols) const = 0;
  virtual Vec2u getGridCoord(Vec2 mousePos, Vec2 start, Vec2 delta, size_t rows,
                             size_t cols) const = 0;
};
} // namespace GridFactories

#endif // GRID_ABSTRACT_FACTORY_HPP