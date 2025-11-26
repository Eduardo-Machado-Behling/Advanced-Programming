#include "Cells/PathOrigin.hpp"

#include "Grid/Manager.hpp"
#include "Path/Dijkstra.hpp"

namespace Cells {
PathOrigin::PathOrigin(Vec2u origin, Vec2u destination)
    : m_origin(origin), m_dest(destination) {}

void PathOrigin::draw(Engine::Engine &engine) {
  Vec2 center = GridManager::get().getCenter(m_origin);

  engine.createPoint(center, {0, 1, 0}, 12);
}

// TODO: Path make actual dt
bool PathOrigin::tick(Engine::Engine &engine, double dt) {
  if (dt == 0) {
    m_path = Dijkstra::ShortestPath(m_origin, m_dest,
                                    *GridManager::get().getGraph());

    for (auto p : m_path) {
      std::cout << p[0] << ", " << p[1] << " -> ";
    }
    std::cout << "\n\n";
  }

  size_t maxi = static_cast<size_t>(dt);
  if (maxi > m_path.size()) {
    maxi = m_path.size();
  }

  Vec2 delta = GridManager::get().getCellSize() * 0.5;
  for (size_t i = 1; i < maxi; i++) {
    Vec2 iPos = GridManager::get().getCenter(m_path[i]);
    Vec2 piPos = GridManager::get().getCenter(m_path[i - 1]);
    engine.createLine(piPos, iPos, {1, 1, 0}, 2);
  }

  return maxi == m_path.size();
}

void PathOrigin::clear() { GridManager::get().get(m_dest)->clear(); }

}; // namespace Cells
