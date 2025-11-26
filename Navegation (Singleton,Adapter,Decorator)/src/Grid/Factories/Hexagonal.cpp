#include "Grid/Factories/Hexagonal.hpp"

#include "Grid/HexagonalGrid.hpp"
#include "Path/HexagonalGridAdapter.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>


namespace GridFactories {

Grid::IGrid *HexagonalFactory::createGrid(Vec2u pos) const {
  return new Grid::HexagonalGrid(pos);
}

Grid::IGraph *HexagonalFactory::createGraph() const {
  return new HexagonalGridAdapter();
}

Vec2 HexagonalFactory::calculateDelta(Vec2 areaSize, size_t rows,
                                      size_t cols) const {
  float widthMag = std::abs(areaSize[0]) / cols;
  float heightMag = std::abs(areaSize[1]) / rows;

  const float HEX_SPACING_RATIO = 0.8660254f;

  float dx, dy;
  float dyFromW = widthMag * HEX_SPACING_RATIO;

  if (dyFromW <= heightMag) {
    dx = widthMag;
    dy = dyFromW;
  } else {
    dy = heightMag;
    dx = heightMag / HEX_SPACING_RATIO;
  }

  float signX = (areaSize[0] < 0) ? -1.0f : 1.0f;
  float signY = (areaSize[1] < 0) ? -1.0f : 1.0f;

  return {dx * signX, dy * signY};
}

Vec2u HexagonalFactory::getGridCoord(Vec2 mousePos, Vec2 start, Vec2 delta,
                                     size_t rows, size_t cols) const {
  Vec2 relPos = mousePos - start;

  uint32_t roughRow = static_cast<uint32_t>(relPos[1] / delta[1]);
  if (roughRow >= rows)
    roughRow = rows - 1;

  float offset = (roughRow % 2 != 0) ? delta[0] * 0.5f : 0.0f;
  float effectiveX = (relPos[0] >= offset) ? relPos[0] - offset : 0;

  uint32_t roughCol = static_cast<uint32_t>((relPos[0] - offset) / delta[0]);
  if (roughCol >= cols)
    roughCol = cols - 1;

  Vec2u bestCoord = {roughCol, roughRow};
  float minDst = FLT_MAX;

  for (int rOff = -1; rOff <= 1; rOff++) {
    for (int cOff = -1; cOff <= 1; cOff++) {
      int checkRow = (int)roughRow + rOff;
      int checkCol = (int)roughCol + cOff;

      if (checkRow >= 0 && checkRow < (int)rows && checkCol >= 0 &&
          checkCol < (int)cols) {

        float cX = start[0] + delta[0] * checkCol + delta[0] * 0.5f;
        float cY = start[1] + delta[1] * checkRow + delta[1] * 0.5f;

        if (checkRow % 2 != 0)
          cX += delta[0] * 0.5f;

        float dx = cX - mousePos[0];
        float dy = cY - mousePos[1];
        float dstSq = dx * dx + dy * dy;

        if (dstSq < minDst) {
          minDst = dstSq;
          bestCoord = {(uint32_t)checkCol, (uint32_t)checkRow};
        }
      }
    }
  }
  return bestCoord;
}
} // namespace GridFactories