#ifndef SIMULATION_COLLISION_GRIDSTRATEGY_HPP
#define SIMULATION_COLLISION_GRIDSTRATEGY_HPP

#include "Simulation/Collision/IStrategy.hpp"
#include "Simulation/Collision/ISystem.hpp"

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "Math/Vector.hpp"

namespace Simulation {
class Agent;
}

namespace Simulation {
namespace Collision {

// =========================================================
// GRID SYSTEM (Simplified Reservation Table)
// =========================================================

class GridSystem : public ISystem {
public:
  GridSystem();
  ~GridSystem();

  void preStep(double dt);
  void postStep();

  bool isOccupied(Engine::Math::Vector<2, float> pos, int timeStep,
                  int excludeAgentID = -1);

  void reserve(Engine::Math::Vector<2, float> pos, int timeStep, int agentID);

  bool tryReserve(Engine::Math::Vector<2, float> pos, int timeStep);

  void clearAgentReservations(int agentID);
  uint32_t getWait();

private:
  void syncStaticMap();
  bool isStaticWall(int x, int y);

  uint64_t getCellID(int x, int y);
  uint64_t getCellID(Engine::Math::Vector<2, float> pos);

private:
  std::unordered_map<uint64_t, int> reservationTable;

  std::vector<bool> staticMapCache;
  int cacheRows = 0;
  int cacheCols = 0;
};

// =========================================================
// GRID STRATEGY (Velocity-Based Reservation)
// =========================================================

class GridStrategy : public IStrategy {
public:
  explicit GridStrategy(GridSystem &sys);
  ~GridStrategy();

  Engine::Math::Vector<2, float> computeVelocity(Simulation::Agent *me);

private:
  GridSystem &system;
  std::unordered_map<Agent *, std::function<void()>> processes;
};
} // namespace Collision
} // namespace Simulation
#endif // SIMULATION_COLLISION_GRIDSTRATEGY_HPP