#include "Actions/AddCell.hpp"
#include "Grid/Manager.hpp"

namespace Commands {
AddCell::AddCell(Cells::ICell *cell, Vec2u position)
    : m_cell(cell), m_position(position) {}

void AddCell::execute() {
	if(!GridManager::get().allocated())
		return;

	GridManager::get().get(m_position)->set(m_cell);
}

} // namespace Commands
