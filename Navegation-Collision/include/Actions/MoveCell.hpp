#ifndef MOVE_CELL_HPP
#define MOVE_CELL_HPP

#include "Actions/Command.hpp"
#include "Cells/Cell.hpp"

namespace Commands {
class MoveCell : public Command {
public:
  MoveCell(Vec2u position, Vec2u target);
  ~MoveCell() = default;

  void execute() override;

private:
  Cells::ICell* m_cell;
  Vec2u m_position;
  Vec2u m_target;
};
} // namespace Commands

#endif

