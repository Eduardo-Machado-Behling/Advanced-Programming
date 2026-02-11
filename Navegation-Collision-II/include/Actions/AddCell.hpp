#ifndef ADD_CELL_HPP
#define ADD_CELL_HPP

#include "Actions/Command.hpp"
#include "Math/Vector.hpp"

namespace Grid {
struct IGrid;
}

namespace Cells {
struct ICell;
}

namespace Commands {
class AddCell : public Command {
public:
  AddCell(Cells::ICell *cell, Vec2u position);
  AddCell(Cells::ICell *cell, Grid::IGrid *grid);
  ~AddCell() = default;

  bool execute() override;

private:
  Grid::IGrid *m_grid;
  Cells::ICell *m_cell;
  Vec2u m_position;
};
} // namespace Commands

#endif
