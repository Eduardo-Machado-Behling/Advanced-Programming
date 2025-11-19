#include "Grid/Factories/Square.hpp"

#include "Grid/SquareGrid.hpp"

namespace GridFactories {

Grid::IGrid *SquareFactory::createGrid(Vec2u pos) const {
  return new Grid::SquareGrid(pos);
}
} // namespace GridFactories