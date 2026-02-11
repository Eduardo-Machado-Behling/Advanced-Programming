#ifndef SIMULATOR_COLLISION_ISYSTEM_HPP
#define SIMULATOR_COLLISION_ISYSTEM_HPP

namespace Simulation {
namespace Collision {

class ISystem {
public:
  virtual void preStep(double dt) {}
  virtual void postStep() {}
  virtual ~ISystem() = default;
};

} // namespace Collision
} // namespace Simulation

#endif // SIMULATOR_COLLISIONS_ISYSTEM_HPP