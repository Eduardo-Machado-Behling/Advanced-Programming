#ifndef REMOVE_CELL_HPP
#define REMOVE_CELL_HPP

#include "Actions/Command.hpp"
#include "Cells/Cell.hpp"

namespace Commands {
class RemoveCell : public Command {
public:
  RemoveCell(Vec2u position);
  ~RemoveCell() = default;

  void execute() override;

private:
  std::unique_ptr<Cells::ICell> m_cell;
  Vec2u m_position;
};
} // namespace Commands

#endif
