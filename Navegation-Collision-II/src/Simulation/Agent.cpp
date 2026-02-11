#include "Simulation/Agent.hpp"

#include "Grid/Manager.hpp"
#include "Utils/StatsManager.hpp"

#include "Actions/Invoker.hpp"
#include "Actions/MoveAgent.hpp"

#include <algorithm>
#include <iostream>
#include <optional>

namespace Simulation {

Agent::Agent(Vec2 startPos, float r, float speed, Path goal)
    : position(startPos), velocity({0, 0}), preferredVelocity({0, 0}),
      radius(r), maxSpeed(speed), goal(goal) {
  finalPos = GridManager::get().getCenter(goal.end);
  lastGridPos = this->goal.start;

  GridManager::get().get(goal.start)->getAgents().push_back(this);
}

Agent::~Agent() {
  auto &origAgents = GridManager::get().get(lastGridPos)->getAgents();

  auto it = std::find(origAgents.begin(), origAgents.end(), this);

  if (it != origAgents.end()) {
    *it = origAgents.back();
    origAgents.pop_back();
  }
}

void Agent::setStrategy(std::unique_ptr<Collision::IStrategy> newStrategy) {
  strategy = std::move(newStrategy);
}

void Agent::reset() {
  i = 0;
  velocity = {0, 0};

  static Invoker &ink = Invoker::get();
  ink.addCommand(new Commands::MoveAgent(this, lastGridPos, this->goal.start));
  lastGridPos = this->goal.start;
}

void Agent::draw(Engine::Engine &engine) {}

void Agent::update(double dt) {
  if (!goals.empty() && i >= goals.size()) {
    position = finalPos;
    return;
  }

  Vec2 goal = position;
  if (!goals.empty()) {
    goal = goals[i];
    Vec2 direction = goal - position;
    direction.norm();
    float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
    direction[0] += noise;
    direction[1] += noise;
    direction.norm();

    preferredVelocity = direction * maxSpeed;
  } else {
    preferredVelocity = 0;
  }

  if (strategy) {
    velocity = strategy->computeVelocity(this);
  } else {
    velocity = preferredVelocity;
  }

  Vec2 newPosition = position + (velocity * (float)dt);

  float distSq = (goal - newPosition).magSq();
  float tolerance = 2.0f;

  if (distSq < (tolerance * tolerance)) {
    i++;
  }
  position = newPosition;

  static GridManager &gm = GridManager::get();
  static Invoker &ink = Invoker::get();
  Vec2u gridPos = gm.getCoord(position);

  if (gridPos != lastGridPos) {
    std::cout << "gridMove: " << lastGridPos << " -> " << gridPos << '\n';
    ink.addCommand(new Commands::MoveAgent(this, lastGridPos, gridPos));
    lastGridPos = gridPos;
  }
}

} // namespace Simulation