#include "Actions/AddCell.hpp"
#include "Grid/Manager.hpp"

namespace Commands {

AddCell::AddCell(Cells::ICell *cell, Grid::IGrid *grid)
    : m_cell(cell), m_grid(grid) {}

AddCell::AddCell(Cells::ICell *cell, Vec2u position)
    : m_cell(cell), m_position(position), m_grid(nullptr) {}

bool AddCell::execute() {
  if (!m_grid) {
    if (!GridManager::get().allocated())
      return false;
    m_grid = GridManager::get().get(m_position);
  }

  m_grid->set(m_cell);
  return false;
}

} // namespace Commands
