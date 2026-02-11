#ifndef SIMULATION_MANAGER_HPP
#define SIMULATION_MANAGER_HPP

#include "Simulation/Agent.hpp"
#include "Simulation/Collision/Factory/IAbstract.hpp"
#include <memory>
#include <vector>

// Forward Declaration to avoid including the heavy Engine header here
namespace Engine {
class Engine;
}

namespace Simulation {

class Manager {
  using Agents_t = std::list<std::unique_ptr<Agent>>;

public:
  using AgentID = Agents_t::iterator;

  // Singleton access
  static Manager &get();

  // Prevent cloning
  Manager(const Manager &) = delete;
  Manager &operator=(const Manager &) = delete;

  // Private constructor (optional, but good practice for Singleton)
  Manager() = default;

  void init(std::unique_ptr<Collision::IFactory> factory);
  void reset();

  double getFPS();
  void setFPS(double fps);

  AgentID addAgent(std::unique_ptr<Agent> agent);

  void rmvAgent(AgentID it);

  Agent &getAgent(AgentID it);

  void update(double dt);

private:
  Agents_t agents;

  std::unique_ptr<Collision::IFactory> factory;
  std::unique_ptr<Collision::ISystem> system;

  double m_dt = 0;
  double m_timeStep = 0;
  double m_fps = 0;
};

} // namespace Simulation

#endif // SIMULATION_MANAGER_HPP