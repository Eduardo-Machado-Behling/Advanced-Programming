#ifndef SIMULATION_COLLISION_FACTORY_RVO_HPP
#define SIMULATION_COLLISION_FACTORY_RVO_HPP

#include "Simulation/Collision/Factory/IAbstract.hpp"
#include <memory>

namespace Simulation {
namespace Collision {

class RvoSystem;
class ISystem;
class IStrategy;

class RvoFactory : public IFactory {
  RvoSystem *currentSystem = nullptr;

public:
  std::unique_ptr<ISystem> createSystem() override;
  std::unique_ptr<IStrategy> createStrategy() override;
};

} // namespace Collision
} // namespace Simulation

#endif // SIMULATION_COLLISION_FACTORY_RVO_HPP