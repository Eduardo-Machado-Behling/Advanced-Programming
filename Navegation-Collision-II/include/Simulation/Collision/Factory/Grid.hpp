#ifndef SIMULATION_COLLISION_FACTORY_GRID_HPP
#define SIMULATION_COLLISION_FACTORY_GRID_HPP

#include "Simulation/Collision/Factory/IAbstract.hpp"
#include <memory>

namespace Simulation {
namespace Collision {

// Forward declarations to avoid heavy includes in header
class GridSystem;
class ISystem;
class IStrategy;

class GridFactory : public IFactory {
  GridSystem *currentSystem = nullptr;

public:
  std::unique_ptr<ISystem> createSystem() override;
  std::unique_ptr<IStrategy> createStrategy() override;
};

} // namespace Collision
} // namespace Simulation

#endif // SIMULATION_COLLISION_FACTORY_GRID_HPP