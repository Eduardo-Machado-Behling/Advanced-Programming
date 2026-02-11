#ifndef SIMULATION_AGENT_HPP
#define SIMULATION_AGENT_HPP

#include "Math/Vector.hpp" // Assuming Vec2 is here
#include "Path/Manager.hpp"
#include "Simulation/Collision/IStrategy.hpp"
#include <memory>
#include <vector>

// Forward Declaration:
// Allows us to use Engine& in the draw function without including the huge
// engine.hpp here
namespace Engine {
class Engine;
}

namespace Simulation {

class Agent {
public:
  struct Path {
    Vec2u start;
    Vec2u end;
  };

  // Physics State
  Vec2 position;
  Vec2 finalPos;
  Vec2 velocity;
  Vec2 preferredVelocity;
  float radius;
  float maxSpeed;
  int i = 0; // Waypoint index
  std::vector<Vec2> goals;

  Path goal;
  Vec2u lastGridPos;
  std::unique_ptr<Collision::IStrategy> strategy;
  PathManager::PathPtr pathID;

  // Constructor
  Agent(Vec2 startPos, float r, float speed, Path goal);
  ~Agent();

  // Methods
  void setStrategy(std::unique_ptr<Collision::IStrategy> newStrategy);
  void reset();
  void draw(Engine::Engine &engine);
  void update(double dt);
};

} // namespace Simulation

#endif // SIMULATION_AGENT_HPP