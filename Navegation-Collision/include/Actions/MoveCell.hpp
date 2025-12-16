#ifndef MOVE_CELL_HPP
#define MOVE_CELL_HPP

#include "Actions/Command.hpp"
#include "Cells/Cell.hpp"

#include <memory>

namespace Commands {
class MoveCell : public Command {
public:
  MoveCell(Vec2u position, Vec2u target);
  ~MoveCell() = default;

  bool execute() override;
  void restore() override;

private:
  Cells::ICell *m_orig;
  std::unique_ptr<Cells::ICell> m_dest;
  Vec2u m_position;
  Vec2u m_target;
};
} // namespace Commands

#endif
