#include "Actions/RemoveCell.hpp"

#include "Grid/Manager.hpp"

namespace Commands {
RemoveCell::RemoveCell(Grid::IGrid *grid, bool clear, bool save)
    : m_grid(grid), m_save(save), m_clear(clear) {}
RemoveCell::RemoveCell(Vec2u position, bool clear, bool save)
    : m_grid(nullptr), m_position(position), m_save(save), m_clear(clear) {}

bool RemoveCell::execute() {
  if (!m_grid) {
    if (!GridManager::get().allocated()) {
      return false;
    }
    m_grid = GridManager::get().get(m_position);
  }

  GridManager::get().setFreed(m_position);

  m_cell.reset(m_grid->reset());
  if (m_cell && m_clear)
    m_cell->clear();

  return m_save;
}

void RemoveCell::restore() {
  GridManager::get().get(m_position)->set(m_cell.release());
}

} // namespace Commands
