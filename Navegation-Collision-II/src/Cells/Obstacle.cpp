#include "Cells/Obstacle.hpp"

#include "Grid/Manager.hpp"

namespace Cells {

Obstacle::Obstacle(Vec2u pos) { m_pos = pos; }

void Obstacle::draw(Engine::Engine &engine) {
  GridManager::get().get(m_pos)->fill(Color{0.2, 0.2, 0.2});
}

bool Obstacle::tick(Engine::Engine &engine, double dt) { return true; }

bool Obstacle::isBlocking() { return true; }
} // namespace Cells