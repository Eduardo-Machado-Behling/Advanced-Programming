#ifndef CELL_HPP
#define CELL_HPP

#include "Math/Vector.hpp"
#include "engine.hpp"

namespace Cells {
struct ICell {
public:
  virtual ~ICell() {}

  virtual void draw(Engine::Engine &engine) = 0;
  virtual void tickSetup() {}
  virtual bool tick(Engine::Engine &engine, double dt) = 0;
  virtual void clear() {}
  virtual void reset() {}

  Vec2u getPos() { return m_pos; }

protected:
  Vec2u m_pos;
};

} // namespace Cells

#endif // CELL_HPP