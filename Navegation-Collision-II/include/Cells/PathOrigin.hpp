#ifndef PATH_ORIGIN_HPP
#define PATH_ORIGIN_HPP

#include "Cells/Cell.hpp"
#include "Math/Vector.hpp"
#include "Path/Manager.hpp"

#include "Simulation/Manager.hpp"

namespace Cells {
class PathOrigin final : public ICell {
public:
  PathOrigin(Vec2u origin);
  PathOrigin(Vec2u origin, Vec2u destination);
  PathOrigin(PathOrigin &&other);
  ~PathOrigin();

  void draw(Engine::Engine &engine) override;

  void tickSetup() override;
  bool tick(Engine::Engine &engine, double dt) override;
  void clear() override;
  void reset() override;

private:
  Vec2u m_origin;
  Vec2u m_dest;
  Vec2 m_curr;

  Vec2 m_pos;
  bool m_hasAgent;

  Simulation::Manager::AgentID m_agent;
  PathManager::PathPtr m_path;
};
}; // namespace Cells

#endif // START_HPP
