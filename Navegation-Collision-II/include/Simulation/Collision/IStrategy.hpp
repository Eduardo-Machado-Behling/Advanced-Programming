#ifndef SIMUALTION_COLLISION_ISTRATEGY_HPP
#define SIMUALTION_COLLISION_ISTRATEGY_HPP

#include "Math/Vector.hpp"
#include <vector>

using Vec2 = Engine::Math::Vector<2, float>;

namespace Simulation {
class Agent;

namespace Collision {

class IStrategy {
public:
  virtual ~IStrategy() = default;

  virtual Vec2 computeVelocity(Agent *me) = 0;
};

} // namespace Collision
} // namespace Simulation

#endif // SIMUALTION_COLLISION_ISTRATEGY_HPP