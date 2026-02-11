#ifndef GRID_HPP
#define GRID_HPP

#include "Cells/Cell.hpp"
#include "engine.hpp"

#include "Observer/Publisher.hpp"
#include <vector>

namespace Simulation {
class Agent;
}

namespace Grid {
struct IGrid {
  IGrid(Vec2u coord);
  virtual ~IGrid() = 0;

  void set(Cells::ICell *cell);
  void tickSetup();
  bool tick(Engine::Engine &engine, double dt);
  void clear();
  Cells::ICell *reset();
  std::vector<Simulation::Agent *> &getAgents();
  bool empty() const;
  bool isBlocking() const;
  void fill(Color color);
  void subscribeOnChanged(Subscriber *sub);

  virtual void draw(Engine::Engine &engine) = 0;
  virtual Vec2 center() const = 0;
  virtual Engine::Objects::ObjectUUID::UUID getUUID() const = 0;
  virtual float offsetRow(size_t row) const = 0;

protected:
  std::unique_ptr<Cells::ICell> m_cell = nullptr;
  std::vector<Simulation::Agent *> m_agents;
  Publisher m_cellChanged;

  Color m_fill;
  Vec2u m_coord;
};

} // namespace Grid

#endif // GRID_HPP
