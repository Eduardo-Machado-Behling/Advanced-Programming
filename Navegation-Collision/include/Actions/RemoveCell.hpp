#ifndef REMOVE_CELL_HPP
#define REMOVE_CELL_HPP

#include "Actions/Command.hpp"
#include "Cells/Cell.hpp"

namespace Commands {
class RemoveCell : public Command {
public:
  RemoveCell(Vec2u position, bool clear = true, bool save = false);
  ~RemoveCell() = default;

  bool execute() override;
  void restore() override;

private:
  std::unique_ptr<Cells::ICell> m_cell;
  Vec2u m_position;
  bool m_clear;
  bool m_save;
};
} // namespace Commands

#endif
