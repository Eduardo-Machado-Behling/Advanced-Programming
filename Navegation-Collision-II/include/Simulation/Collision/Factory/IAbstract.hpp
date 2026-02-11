#ifndef SIMULATOR_COLLISION_IABSTRACTFACTORY_HPP
#define SIMULATOR_COLLISION_IABSTRACTFACTORY_HPP

#include "Simulation/Collision/IStrategy.hpp"
#include "Simulation/Collision/ISystem.hpp"
#include <memory>

namespace Simulation {

namespace Collision {

class IFactory {
public:
  virtual ~IFactory() = default;

  // 1. Create the Global System (Run once at Sim initialization)
  virtual std::unique_ptr<ISystem> createSystem() = 0;

  // 2. Create the Strategy for a single Agent (Run for every new agent)
  virtual std::unique_ptr<IStrategy> createStrategy() = 0;
};
} // namespace Collision

} // namespace Simulation
#endif // SIMULATOR_COLLISION_IFACTORY_HPP