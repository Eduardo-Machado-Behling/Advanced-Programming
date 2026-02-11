#include "Simulation/Collision/Factory/Grid.hpp"
#include "Simulation/Collision/GridStrategy.hpp"

namespace Simulation {
namespace Collision {

std::unique_ptr<ISystem> GridFactory::createSystem() {
  auto sys = std::make_unique<GridSystem>();
  currentSystem = sys.get();
  return sys;
}

std::unique_ptr<IStrategy> GridFactory::createStrategy() {

  if (!currentSystem) {

    return nullptr;
  }
  return std::make_unique<GridStrategy>(*currentSystem);
}

} // namespace Collision
} // namespace Simulation