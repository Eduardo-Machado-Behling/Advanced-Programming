#include "Grid/SquareGrid.hpp"

#include "Grid/Manager.hpp"

namespace Grid {

SquareGrid::SquareGrid(Vec2u coord) : IGrid(coord) {}

void SquareGrid::draw(Engine::Engine &engine) {
  Vec2 gridSize = GridManager::get().getCellSize();
  Vec2 start = GridManager::get().start();

  start[0] += gridSize[0] * m_coord[1];
  start[1] += gridSize[1] * m_coord[0];
  Vec2 corners[4] = {start, start, start, start};

  corners[1][0] += gridSize[0];
  corners[2][1] += gridSize[1];

  corners[3][0] += gridSize[0];
  corners[3][1] += gridSize[1];

  const Color LINE_COLOR = {1, 1, 1};
  const float LINE_STROKE = 2;

  engine.createLine(corners[0], corners[1], LINE_COLOR, LINE_STROKE);
  engine.createLine(corners[0], corners[2], LINE_COLOR, LINE_STROKE);
  engine.createLine(corners[1], corners[3], LINE_COLOR, LINE_STROKE);
  engine.createLine(corners[2], corners[3], LINE_COLOR, LINE_STROKE);

  if (m_cell)
    m_cell->draw(engine);

  std::vector<Vec2> verts(corners, corners + 4);
  engine.createPoly(verts, m_fill, m_fill, 0, false);
}

Vec2 SquareGrid::center() const {
  Vec2 gridSize = GridManager::get().getCellSize();
  Vec2 start = GridManager::get().start();

  start[0] += gridSize[0] * m_coord[1];
  start[1] += gridSize[1] * m_coord[0];

  return start + gridSize * 0.5f;
}

float SquareGrid::offsetRow(size_t row) const { return 0; }
} // namespace Grid
