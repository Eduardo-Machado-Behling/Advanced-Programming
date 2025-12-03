#include "Actions/RemoveCell.hpp"

#include "Grid/Manager.hpp"

namespace Commands {
RemoveCell::RemoveCell(Vec2u position) : m_position(position) {}

void RemoveCell::execute() {
  if (!GridManager::get().allocated()) {
    return;
  }

  m_cell.reset(GridManager::get().get(m_position)->reset());
}

} // namespace Commands
