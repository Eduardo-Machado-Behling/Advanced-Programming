#include "Simulation/Collision/GridStrategy.hpp"
#include "Grid/Manager.hpp"
#include "Simulation/Agent.hpp"
#include "Utils/StatsManager.hpp"

#include <cmath>
#include <unordered_map>

namespace Simulation {
namespace Collision {

GridSystem::GridSystem() {}

GridSystem::~GridSystem() {}

void GridSystem::preStep(double dt) {

  reservationTable.clear();

  if (staticMapCache.empty()) {
    syncStaticMap();
  }
}

void GridSystem::postStep() {}

bool GridSystem::isOccupied(Engine::Math::Vector<2, float> pos, int timeStep,
                            int excludeAgentID) {
  uint64_t id = getCellID(pos);
  Vec2u coord = GridManager::get().getCoord(pos);

  if (isStaticWall(coord[0], coord[1]))
    return true;

  if (reservationTable.count(id)) {
    if (reservationTable[id] != excludeAgentID) {
      return true;
    }
  }
  return false;
}

void GridSystem::reserve(Engine::Math::Vector<2, float> pos, int timeStep,
                         int agentID) {
  reservationTable[getCellID(pos)] = agentID;
}

void GridSystem::clearAgentReservations(int agentID) {}
uint32_t GridSystem::getWait() { return 0; }

void GridSystem::syncStaticMap() {
  auto &gm = GridManager::get();
  cacheRows = gm.rows();
  cacheCols = gm.cols();
  staticMapCache.resize(cacheRows * cacheCols);
  for (int y = 0; y < cacheRows; ++y) {
    for (int x = 0; x < cacheCols; ++x) {
      staticMapCache[y * cacheCols + x] = gm.get(y, x)->isBlocking();
    }
  }
}

bool GridSystem::isStaticWall(int x, int y) {
  if (x < 0 || x >= cacheCols || y < 0 || y >= cacheRows)
    return true;
  return staticMapCache[y * cacheCols + x];
}

uint64_t GridSystem::getCellID(int x, int y) {
  return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) |
         static_cast<uint32_t>(y);
}

uint64_t GridSystem::getCellID(Engine::Math::Vector<2, float> pos) {
  Vec2u coord = GridManager::get().getCoord(pos);
  return getCellID(coord[0], coord[1]);
}

bool GridSystem::tryReserve(Engine::Math::Vector<2, float> pos, int timeStep) {
  return true;
}

GridStrategy::GridStrategy(GridSystem &sys) : system(sys) {}

GridStrategy::~GridStrategy() {}

Vec2 GridStrategy::computeVelocity(Simulation::Agent *me) {
  if (processes.find(me) == processes.end()) {
    auto processPath = [me]() {
      PathManager &pm = PathManager::get();
      size_t i = 1;

      me->goals.clear();
      for (; true; i++) {
        std::optional<Vec2> goal = pm.getSegment(me->pathID.get(), i);

        if (!goal) {
          break;
        }
        me->goals.push_back(goal.value());
      }

      if (std::get<bool>(
              StatsManager::get().get("midPointPath").value_or(false))) {
        Vec2 lastPos = pm.getSegment(me->pathID.get(), 0).value();
        for (size_t i = 1; i < me->goals.size(); i++) {
          Vec2 goal = me->goals[i];
          me->goals[i] = (me->goals[i] + lastPos) * 0.5;
          lastPos = goal;
        }

        me->goals.push_back(lastPos);
      }
    };

    processes.insert({me, processPath});
  }

  if (me->goals.empty()) {
    PathManager &pm = PathManager::get();
    me->pathID = pm.requestPath({me->maxSpeed, me->radius}, me->goal.start,
                                me->goal.end);

    auto processPath = processes.at(me);
    processPath();
    pm.registerOnChange(me->pathID.get(), processPath);
  }
  int agentID = static_cast<int>(reinterpret_cast<uintptr_t>(me));

  Vec2 desiredVel = me->preferredVelocity;

  if (desiredVel.magSq() < 0.001f) {
    system.reserve(me->position, 0, agentID);
    return {0, 0};
  }

  Vec2 nextPos = me->position + (desiredVel * 0.1f);

  if (system.isOccupied(nextPos, 0, agentID)) {

    system.reserve(me->position, 0, agentID);
    return {0, 0};
  }

  system.reserve(nextPos, 0, agentID);

  return desiredVel;
}

} // namespace Collision
} // namespace Simulation