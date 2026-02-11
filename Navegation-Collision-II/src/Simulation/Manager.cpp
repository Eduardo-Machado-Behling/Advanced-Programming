#include "Simulation/Manager.hpp"

#include "Utils/StatsManager.hpp"
#include "engine.hpp"

#include "Tracy.hpp"
#include <algorithm>
#include <iterator>

namespace Simulation {

Manager &Manager::get() {
  static Manager *m_instance = new Manager();
  return *m_instance;
}

void Manager::init(std::unique_ptr<Collision::IFactory> factory) {

  this->factory = std::move(factory);

  this->system = this->factory->createSystem();

  for (auto &agent : agents) {
    agent->setStrategy(this->factory->createStrategy());
  }
}

void Manager::reset() {
  for (auto &agent : agents) {
    agent->reset();
  }
}

double Manager::getFPS() { return m_fps; }
void Manager::setFPS(double fps) {
  m_fps = fps;
  m_timeStep = 1 / fps;
}

Manager::AgentID Manager::addAgent(std::unique_ptr<Agent> agent) {

  if (factory) {
    agent->setStrategy(this->factory->createStrategy());
  }

  agents.push_front(std::move(agent));
  return agents.begin();
}

void Manager::rmvAgent(AgentID it) { agents.erase(it); }

Agent &Manager::getAgent(AgentID it) { return **it; }

void Manager::update(double dt) {
  if (!system)
    return;

  size_t iter = 1;
  if (m_fps > 0) {
    m_dt += dt;
    if (m_dt < m_timeStep)
      return;

    iter = 0;
    while (m_dt > m_timeStep) {
      m_dt -= m_timeStep;
      iter++;
    }
    dt = m_timeStep;
  }

  StatsManager::get().set("fpsSim", 1 / dt);
  StatsManager::get().set("iterSim", iter);

  for (size_t i = 0; i < iter; i++) {

    {
      ZoneScopedN("Sim::preStep");
      system->preStep(dt);
    }

    for (auto &agent : agents) {

      {
        ZoneScopedN("Sim::update");
        agent->update(dt);
      }
    }

    {
      ZoneScopedN("Sim::postStep");
      system->postStep();
    }
  }
}

} // namespace Simulation