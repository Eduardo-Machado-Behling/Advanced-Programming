#ifndef HEXAGONAL_GRID_FACTORY_HPP
#define HEXAGONAL_GRID_FACTORY_HPP

#include "Grid/Factories/Abstract.hpp"
#include "Grid/Graph.hpp"

namespace GridFactories {
class HexagonalFactory : public IFactory {
public:
  Grid::IGrid *createGrid(Vec2u pos) const override;
  Grid::IGraph *createGraph() const override;

  Vec2 calculateDelta(Vec2 areaSize, size_t rows, size_t cols) const;

  Vec2u getGridCoord(Vec2 mousePos, Vec2 start, Vec2 delta, size_t rows,
                     size_t cols) const;
};
} // namespace GridFactories

#endif // HEXAGONAL_GRID_FACTORY_HPP