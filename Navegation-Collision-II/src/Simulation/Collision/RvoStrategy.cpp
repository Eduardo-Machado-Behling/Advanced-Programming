#include "Simulation/Collision/RvoStrategy.hpp"

#include "Utils/StatsManager.hpp"

namespace Simulation {
namespace Collision {

RvoStrategy::RvoStrategy(RvoSystem &sys) : system(sys) {}

Vec2 RvoStrategy::computeVelocity(Agent *me) {
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
  RVO::Vector2 pos = {me->position[0], me->position[1]};

  if (!initialized) {
    rvoAgentID = system->addAgent(pos);

    system->setAgentRadius(rvoAgentID, me->radius);
    system->setAgentMaxSpeed(rvoAgentID, me->maxSpeed);

    initialized = true;
    return Vec2({0, 0});
  }

  system->setAgentPosition(rvoAgentID, pos);

  RVO::Vector2 prefVel = {me->preferredVelocity[0], me->preferredVelocity[1]};
  system->setAgentPrefVelocity(rvoAgentID, prefVel);

  RVO::Vector2 result = system->getAgentVelocity(rvoAgentID);

  return {result.x(), result.y()};
}

RvoSystem::RvoSystem() {
  sim.setAgentDefaults(30.0f, 10, 10.0f, 10.0f, 1.5f, 2.0f, {0.0f, 0.0f});
}

void RvoSystem::preStep(double dt) { sim.setTimeStep(dt); }

void RvoSystem::postStep() { sim.doStep(); }

RVO::RVOSimulator *RvoSystem::operator->() { return &sim; }
} // namespace Collision
} // namespace Simulation