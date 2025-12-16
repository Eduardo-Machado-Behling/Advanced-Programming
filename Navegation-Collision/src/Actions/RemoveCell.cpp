#include "Actions/RemoveCell.hpp"

#include "Grid/Manager.hpp"

namespace Commands {
RemoveCell::RemoveCell(Vec2u position, bool clear, bool save)
    : m_position(position), m_save(save), m_clear(clear) {}

bool RemoveCell::execute() {
  if (!GridManager::get().allocated()) {
    return false;
  }

  Grid::IGrid *grid = GridManager::get().get(m_position);

  m_cell.reset(grid->reset());
  if (m_cell && m_clear)
    m_cell->clear();

  return m_save;
}

void RemoveCell::restore() {
  GridManager::get().get(m_position)->set(m_cell.release());
}

} // namespace Commands
