#include "Actions/MoveAgent.hpp"

#include "Actions/Invoker.hpp"
#include "Grid/Manager.hpp"

#include <algorithm>

namespace Commands {
MoveAgent::MoveAgent(Simulation::Agent *agent, Vec2u last, Vec2u now)
    : m_agent(agent), m_last(last), m_now(now) {}

bool MoveAgent::execute() {
  if (!GridManager::get().allocated() || m_last == m_now) {
    return false;
  }

  Grid::IGrid *orig = GridManager::get().get(m_last);
  Grid::IGrid *dest = GridManager::get().get(m_now);

  std::vector<Simulation::Agent *> &origAgents = orig->getAgents();
  auto it = std::find(origAgents.begin(), origAgents.end(), m_agent);

  if (it != origAgents.end()) {
    *it = origAgents.back();
    origAgents.pop_back();
  }

  dest->getAgents().push_back(m_agent);

  return false;
}

void MoveAgent::restore() {}

} // namespace Commands
