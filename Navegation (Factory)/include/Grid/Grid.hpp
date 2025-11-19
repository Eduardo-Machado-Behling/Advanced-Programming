#ifndef GRID_HPP
#define GRID_HPP

#include "Cells/Cell.hpp"
#include "engine.hpp"

namespace Grid {
struct IGrid {
  IGrid(Vec2u coord);
  virtual ~IGrid() = 0;

  void set(Cells::ICell *cell);
  bool tick(Engine::Engine &engine, double dt);
  void clear();
  bool empty();
  void fill(Color color);

  virtual void draw(Engine::Engine &engine) = 0;

protected:
  std::unique_ptr<Cells::ICell> m_cell = nullptr;
  Color m_fill;
  Vec2u m_coord;
};

} // namespace Grid

#endif // GRID_HPP