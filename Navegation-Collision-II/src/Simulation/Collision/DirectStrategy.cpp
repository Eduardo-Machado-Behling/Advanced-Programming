#include "Simulation/Collision/DirectStrategy.hpp"
#include "Simulation/Agent.hpp"

#include "Grid/Manager.hpp"
#include "Utils/StatsManager.hpp"

#include <cmath>

namespace Simulation {
namespace Collision {

void DirectSystem::preStep(double dt) {}

void DirectSystem::postStep() {}

DirectStrategy::DirectStrategy(DirectSystem &sys) : system(sys) {}

Vec2 DirectStrategy::computeVelocity(Simulation::Agent *me) {
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

  static GridManager &gm = GridManager::get();

  Vec2 avoidanceForce = {0, 0};
  int threatCount = 0;

  Vec2u pos = gm.getCoord(me->position);

  auto neighborsCoord = gm.getGraph()->getNeighbors(pos);
  neighborsCoord.push_back(pos);

  for (auto neighborCoord : neighborsCoord) {
    auto *cell = gm.get(neighborCoord);
    if (!cell)
      continue;

    auto &neighbors = cell->getAgents();
    for (Agent *neighbor : neighbors) {

      if (neighbor == me)
        continue;

      Vec2 toNeighbor = neighbor->position - me->position;
      float dist = toNeighbor.mag();
      float combinedRadius = me->radius + neighbor->radius;

      if (dist < combinedRadius * 3.0f && dist > 0.001f) {

        Vec2 pushDir = toNeighbor * -1.0f;
        pushDir.norm();

        float avoidanceRange = combinedRadius * 3.0f;

        float factor = (avoidanceRange - dist) / avoidanceRange;
        if (factor < 0)
          factor = 0;

        float strength = factor * (me->maxSpeed * 2.0f);

        avoidanceForce += pushDir * strength;
        threatCount++;
      }
    }
  }

  Vec2 finalVel = me->preferredVelocity;

  if (threatCount > 0) {
    finalVel += avoidanceForce;

    if (finalVel.magSq() > me->maxSpeed * me->maxSpeed) {
      finalVel.norm();
      finalVel *= me->maxSpeed;
    }
  }

  return finalVel;
}

} // namespace Collision
} // namespace Simulation