#include "Cells/PathDestination.hpp"
#include "Grid/Manager.hpp"

namespace Cells {
PathDestination::PathDestination(Vec2u destination, Vec2u origin)
    : m_dest(destination), m_origin(origin) {}

void PathDestination::draw(Engine::Engine &engine) {
  Vec2 center = GridManager::get().getCenter(m_dest);

  engine.createPoint(center, {1, 0, 0}, 12);
}

bool PathDestination::tick(Engine::Engine &engine, double dt) { return true; }

void PathDestination::clear() { GridManager::get().get(m_origin)->clear(); }

}; // namespace Cells
