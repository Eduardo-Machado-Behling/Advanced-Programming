#include "Simulation/Collision/Factory/Rvo.hpp"
#include "Simulation/Collision/RvoStrategy.hpp"

namespace Simulation {
namespace Collision {

std::unique_ptr<ISystem> RvoFactory::createSystem() {
  auto sys = std::make_unique<RvoSystem>();
  currentSystem = sys.get();
  return sys;
}

std::unique_ptr<IStrategy> RvoFactory::createStrategy() {
  if (!currentSystem) {
    return nullptr;
  }
  return std::make_unique<RvoStrategy>(*currentSystem);
}

} // namespace Collision
} // namespace Simulation