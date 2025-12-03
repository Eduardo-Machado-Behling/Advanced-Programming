#include "Grid/Factories/Square.hpp"

#include "Grid/SquareGrid.hpp"
#include "Path/SquareGridAdapter.hpp"

namespace GridFactories {

Grid::IGrid *SquareFactory::createGrid(Vec2u pos) const {
  return new Grid::SquareGrid(pos);
}

Grid::IGraph *SquareFactory::createGraph() const {
  return new SquareGridAdapter();
}

Vec2 SquareFactory::calculateDelta(Vec2 areaSize, size_t rows,
                                   size_t cols) const {
  return {areaSize[0] / cols, areaSize[1] / rows};
}

Vec2u SquareFactory::getGridCoord(Vec2 mousePos, Vec2 start, Vec2 delta,
                                  size_t rows, size_t cols) const {
  Vec2 relPos = mousePos - start;

  uint32_t col = static_cast<uint32_t>(relPos[0] / delta[0]);
  uint32_t row = static_cast<uint32_t>(relPos[1] / delta[1]);

  if (col >= cols)
    col = cols - 1;
  if (row >= rows)
    row = rows - 1;

  return {col, row};
}
} // namespace GridFactories