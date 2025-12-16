#include "Actions/MoveCell.hpp"

#include "Grid/Manager.hpp"

namespace Commands {
MoveCell::MoveCell(Vec2u position, Vec2u target)
    : m_position(position), m_target(target) {}

void MoveCell::execute() {
  if (!GridManager::get().allocated() || m_position == m_target) {
    return;
  }

  std::cout << "Moved\n";
  m_cell = GridManager::get().get(m_position)->reset();
  GridManager::get().get(m_target)->set(m_cell);
}

} // namespace Commands
