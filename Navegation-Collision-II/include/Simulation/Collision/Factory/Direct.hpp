#ifndef SIMULATION_COLLISION_FACTORY_DIRECT_HPP
#define SIMULATION_COLLISION_FACTORY_DIRECT_HPP

#include "Simulation/Collision/Factory/IAbstract.hpp"
#include <memory>

namespace Simulation {
namespace Collision {

class DirectSystem;
class ISystem;
class IStrategy;

class DirectFactory : public IFactory {
  DirectSystem *currentSystem = nullptr;

public:
  std::unique_ptr<ISystem> createSystem() override;
  std::unique_ptr<IStrategy> createStrategy() override;
};

} // namespace Collision
} // namespace Simulation

#endif // SIMULATION_COLLISION_FACTORY_DIRECT_HPP