#ifndef SIMULATION_COLLISION_RVOSTRATEGY_HPP
#define SIMULATION_COLLISION_RVOSTRATEGY_HPP

#include "Simulation/Collision/IStrategy.hpp"
#include "Simulation/Collision/ISystem.hpp"
#include "Simulation/Manager.hpp"

#include <RVO.h>
#include <functional>
#include <unordered_map>

namespace Simulation {
namespace Collision {

class RvoSystem : public ISystem {
  RVO::RVOSimulator sim;

public:
  RvoSystem();

  void preStep(double dt) override;

  void postStep() override;

  RVO::RVOSimulator *operator->();
};

// =========================================================
// 1. NO COMMUNICATION (RVO2 Library Wrapper)
// Reference: Van den Berg et al. (ORCA)
// =========================================================
class RvoStrategy : public IStrategy {
  RvoSystem &system;
  int rvoAgentID = -1;
  bool initialized = false;
  std::unordered_map<Agent *, std::function<void()>> processes;

public:
  RvoStrategy(RvoSystem &sys);

  Vec2 computeVelocity(Agent *me) override;
};

} // namespace Collision

} // namespace Simulation

#endif // SIMULATION_COLLISION_RVOSTRATEGY_HPP