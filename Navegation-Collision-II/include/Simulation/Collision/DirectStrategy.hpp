#ifndef SIMULATION_COLLISION_DIRECTSTRATEGY_HPP
#define SIMULATION_COLLISION_DIRECTSTRATEGY_HPP

#include "Simulation/Collision/IStrategy.hpp"
#include "Simulation/Collision/ISystem.hpp"

#include <functional>
#include <unordered_map>
#include <vector>

namespace Simulation {

// Forward Declaration
class Agent;

namespace Collision {

// =========================================================
// DIRECT SYSTEM (Pass-through)
// =========================================================
class DirectSystem : public ISystem {
public:
  void preStep(double dt) override;
  void postStep() override;
};

// =========================================================
// STRATEGY: DIRECT COMMUNICATION (Geometric Negotiation)
// Reference: Boardman et al. (2021) - Collision Cones
// =========================================================
class DirectStrategy : public IStrategy {
  DirectSystem &system;
  std::unordered_map<Agent *, std::function<void()>> processes;

public:
  // Constructor
  DirectStrategy(DirectSystem &sys);

  // Main logic override
  Vec2 computeVelocity(Simulation::Agent *me) override;
};

} // namespace Collision
} // namespace Simulation

#endif // SIMULATION_COLLISION_DIRECTSTRATEGY_HPP