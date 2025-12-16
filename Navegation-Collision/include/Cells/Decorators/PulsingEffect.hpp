#include "Cells/Decorators/CellDecorator.hpp"
#include <cmath>

#include "Grid/Manager.hpp"

namespace Cells {

class PulsingDecorator : public CellDecorator {
private:
  double m_time;
  bool ticked = false;
  int i = 0;

public:
  PulsingDecorator(ICell *cell) : CellDecorator(cell), m_time(0.0) {}

  bool tick(Engine::Engine &engine, double dt) override {
    if (dt != 0) {
      m_time += dt;
    } else {
      m_time = 0;
    }

    CellDecorator::tick(engine, dt);
    ticked = true;

    return true;
  }

  void draw(Engine::Engine &engine) override {
    CellDecorator::draw(engine);

    Vec2 center = GridManager::get().getCenter(m_wrappedCell->getPos());

    float radius = 10.0f + 15.0f * std::sin(m_time);

    if (m_time != 0 && ticked) {
      engine.createPoint(center, {1.0f, 0.0f, 0.0f}, radius);
    }

    ticked = false;
  }
};

} // namespace Cells