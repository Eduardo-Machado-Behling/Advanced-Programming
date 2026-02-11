#include "Actions/MoveCell.hpp"

#include "Actions/Invoker.hpp"
#include "Actions/RemoveCell.hpp"
#include "Grid/Manager.hpp"

namespace Commands {
MoveCell::MoveCell(Vec2u position, Vec2u target)
    : m_position(position), m_target(target) {}

bool MoveCell::execute() {
  if (!GridManager::get().allocated() || m_position == m_target) {
    return false;
  }

  std::cout << "Moved\n";

  Grid::IGrid *orig = GridManager::get().get(m_position);
  Grid::IGrid *dest = GridManager::get().get(m_target);
  m_orig = orig->reset();
  m_dest.reset(dest->reset());
  dest->set(m_orig);

  return true;
}

void MoveCell::restore() {
  Grid::IGrid *orig = GridManager::get().get(m_position);
  Grid::IGrid *dest = GridManager::get().get(m_target);
  m_orig = dest->reset();

  if (m_dest)
    m_dest->reset();
  if (m_orig)
    m_orig->reset();

  dest->set(m_dest.release());
  orig->set(m_orig);
}

} // namespace Commands
