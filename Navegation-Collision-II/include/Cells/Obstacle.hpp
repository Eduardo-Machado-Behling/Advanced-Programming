#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "Cells/Cell.hpp"
#include "Math/Vector.hpp"

namespace Cells {
class Obstacle final : public ICell {
public:
  Obstacle(Vec2u pos);

  void draw(Engine::Engine &engine) override;
  bool tick(Engine::Engine &engine, double dt) override;
  bool isBlocking() override;
};
}; // namespace Cells

#endif // OBSTACLE_HPP