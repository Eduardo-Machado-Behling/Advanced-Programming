#include "Grid/Grid.hpp"

namespace Grid {
IGrid::IGrid(Vec2u coord) : m_coord(coord) {}
IGrid::~IGrid() {}

void IGrid::set(Cells::ICell *cell) { m_cell.reset(cell); }
bool IGrid::tick(Engine::Engine &engine, double dt) {
  if (m_cell)
    return m_cell->tick(engine, dt);
  return true;
}
void IGrid::clear() {
  if (m_cell) {
    Cells::ICell *cell = m_cell.release();
    cell->clear();
  }

  m_cell.reset(nullptr);
  m_fill = {0, 0, 0};
}
bool IGrid::empty() const { return !m_cell; }
void IGrid::fill(Color color) { m_fill = color; }
} // namespace Grid
