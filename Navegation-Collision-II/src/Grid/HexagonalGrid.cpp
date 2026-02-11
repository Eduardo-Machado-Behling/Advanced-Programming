#include "Grid/HexagonalGrid.hpp"
#include "Grid/Manager.hpp"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Grid {

HexagonalGrid::HexagonalGrid(Vec2u coord) : IGrid(coord) {}

void HexagonalGrid::draw(Engine::Engine &engine) {
  Vec2 gridSize = GridManager::get().getCellSize();
  Vec2 start = GridManager::get().start();

  size_t col = m_coord[1];
  size_t row = m_coord[0];

  float posX = start[0] + gridSize[0] * col;
  float posY = start[1] + gridSize[1] * row;

  if (row % 2 != 0) {
    posX += gridSize[0] * 0.5f;
  }

  Vec2 center = {posX + gridSize[0] * 0.5f, posY + gridSize[1] * 0.5f};

  float radiusY = gridSize[1] / 1.5f;
  float radiusX = gridSize[0] / 1.7320508f;

  std::vector<Vec2> verts;
  verts.reserve(6);

  for (int i = 0; i < 6; ++i) {
    float angle_deg = 60.0f * i - 30.0f;
    float angle_rad = angle_deg * (M_PI / 180.0f);

    verts.push_back({center[0] + radiusX * std::cos(angle_rad),
                     center[1] + radiusY * std::sin(angle_rad)});
  }

  const Color LINE_COLOR = {1, 1, 1};
  const float LINE_STROKE = 2;

  for (size_t i = 0; i < 6; ++i) {
    size_t next = (i + 1) % 6;
    engine.createLine(verts[i], verts[next], LINE_COLOR, LINE_STROKE);
  }

  poly = &engine.createPoly(verts, m_fill, m_fill, 0, true);

  if (m_cell) {
    m_cell->draw(engine);
  }
}

Vec2 HexagonalGrid::center() const {
  Vec2 gridSize = GridManager::get().getCellSize();
  Vec2 start = GridManager::get().start();

  size_t col = m_coord[1];
  size_t row = m_coord[0];

  float posX = start[0] + gridSize[0] * col;
  float posY = start[1] + gridSize[1] * row;

  if (row % 2 != 0) {
    posX += gridSize[0] * 0.5f;
  }

  return {posX + gridSize[0] * 0.5f, posY + gridSize[1] * 0.5f};
}

Engine::Objects::ObjectUUID::UUID HexagonalGrid::getUUID() const {
  return poly->getUUID();
}

float HexagonalGrid::offsetRow(size_t row) const {
  return (row % 2 != 0) ? GridManager::get().getCellSize()[0] * 0.5f : 0.0f;
}

} // namespace Grid
