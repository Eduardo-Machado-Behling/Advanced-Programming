#include "Cells/PathOrigin.hpp"

#include "Actions/AddCell.hpp"
#include "Actions/Invoker.hpp"
#include "Actions/MoveCell.hpp"
#include "Grid/Manager.hpp"
#include "Path/Dijkstra.hpp"
#include "Path/Manager.hpp"

namespace Cells {
PathOrigin::PathOrigin(Vec2u origin, Vec2u destination)
    : m_origin(origin), m_dest(destination) {
  m_path = PathManager::get().getId({.velocity = 1, .size = {10, 10}});
  m_pos = GridManager::get().getCenter(origin);
  m_curr = origin;
}

void PathOrigin::draw(Engine::Engine &engine) {
  engine.createPoint(m_pos, {0, 1, 0}, 12);
}

// TODO: Path make actual dt
bool PathOrigin::tick(Engine::Engine &engine, double dt) {
  if (dt == 0) {
    PathManager::get().getPath(m_path.get(), m_origin, m_dest,
                               *GridManager::get().getGraph());
    m_pos = GridManager::get().getCenter(m_origin);
    m_curr = m_origin;
    return false;
  }

  auto res = PathManager::get().evalPosition(dt, m_path.get());
  m_pos = res.position;

  if (m_curr != res.grid) {
    Invoker::get().addCommand(new Commands::MoveCell(m_curr, res.grid));
  }
  
  m_curr = res.grid;
  return res.ended;
}

void PathOrigin::clear() { GridManager::get().get(m_dest)->clear(); }

}; // namespace Cells
