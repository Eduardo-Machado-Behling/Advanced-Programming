#include "Simulation/Collision/Factory/Direct.hpp"
#include "Simulation/Collision/DirectStrategy.hpp"

namespace Simulation {
namespace Collision {

std::unique_ptr<ISystem> DirectFactory::createSystem() {
  auto sys = std::make_unique<DirectSystem>();
  currentSystem = sys.get();
  return sys;
}

std::unique_ptr<IStrategy> DirectFactory::createStrategy() {
  if (!currentSystem) {
    return nullptr;
  }
  return std::make_unique<DirectStrategy>(*currentSystem);
}

} // namespace Collision
} // namespace Simulation