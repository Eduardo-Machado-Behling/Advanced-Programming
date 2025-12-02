#ifndef ADD_CELL_HPP
#define ADD_CELL_HPP

#include "Actions/Command.hpp"
#include "Cells/Cell.hpp"
#include "Math/Vector.hpp"

namespace Commands {
class AddCell : public Command {
public:
  AddCell(Cells::ICell *cell, Vec2u position);
  ~AddCell() = default;

  void execute() override;

private:
  Cells::ICell* m_cell;
  Vec2u m_position;
};
} // namespace Commands

#endif
