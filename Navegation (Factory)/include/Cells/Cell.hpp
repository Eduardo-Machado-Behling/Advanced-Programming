#ifndef CELL_HPP
#define CELL_HPP

#include "Math/Vector.hpp"
#include "engine.hpp"

namespace Cells {
struct ICell {
public:
  virtual ~ICell() {}

  virtual void draw(Engine::Engine &engine) = 0;
  virtual bool tick(Engine::Engine &engine, double dt) = 0;
  virtual void clear() {}
};

} // namespace Cells

#endif // CELL_HPP