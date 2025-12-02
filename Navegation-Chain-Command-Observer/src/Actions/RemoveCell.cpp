#include "Actions/RemoveCell.hpp"
#include "Grid/Manager.hpp"

namespace Commands {
RemoveCell::RemoveCell(Vec2u position)
    : m_position(position) {
		m_cell.reset(GridManager::get().get(position)->reset());
	}

void RemoveCell::execute() {
  if (!GridManager::get().allocated()) {
    return;
  }

  GridManager::get().get(m_position)->clear();
}

} // namespace Commands
