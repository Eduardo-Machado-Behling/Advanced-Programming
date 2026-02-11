#include "Cells/PathOrigin.hpp"

#include "Actions/AddCell.hpp"
#include "Actions/Invoker.hpp"
#include "Actions/MoveCell.hpp"
#include "Actions/RemoveCell.hpp"

#include "Grid/Manager.hpp"
#include "Path/Dijkstra.hpp"
#include "Path/Manager.hpp"

#include "Utils/StatsManager.hpp"

namespace Cells {
PathOrigin::PathOrigin(Vec2u origin)
    : m_origin(origin), m_dest({0, 0}), m_hasAgent(false) {
  m_pos = GridManager::get().getCenter(origin);
}

PathOrigin::PathOrigin(Vec2u origin, Vec2u destination)
    : m_origin(origin), m_dest(destination), m_hasAgent(true) {
  m_pos = GridManager::get().getCenter(origin);

  auto &pathMgr = PathManager::get();
  auto &simMgr = Simulation::Manager::get();
  auto &grid = GridManager::get();
  auto &stats = StatsManager::get();

  float radius = std::get<double>(stats.get("agentsRadius").value_or(10.0));
  float speed = std::get<double>(stats.get("agentsSpeed").value_or(30.0));

  auto agent = std::make_unique<Simulation::Agent>(
      m_pos, radius, speed, Simulation::Agent::Path{m_origin, m_dest});

  m_agent = simMgr.addAgent(std::move(agent));
}
PathOrigin::~PathOrigin() {
  if (m_hasAgent)
    Simulation::Manager::get().rmvAgent(m_agent);
}

void PathOrigin::draw(Engine::Engine &engine) {
  engine.createPoint(m_pos, {0, 1, 0}, 12);
  if (m_hasAgent)
    engine.createPoint(m_curr, {1, 0, 1}, 12);
}

void PathOrigin::tickSetup() {
  m_pos = GridManager::get().getCenter(m_origin);
  m_curr = m_pos;
  if (m_hasAgent)
    Simulation::Manager::get().getAgent(m_agent).position = m_pos;
}

bool PathOrigin::tick(Engine::Engine &engine, double dt) {

  if (m_hasAgent)
    m_curr = Simulation::Manager::get().getAgent(m_agent).position;
  return false;
}

void PathOrigin::clear() {
  Invoker::get().addCommand(new Commands::RemoveCell(m_dest, false));
}
void PathOrigin::reset() { m_pos = GridManager::get().getCenter(m_origin); }

}; // namespace Cells
