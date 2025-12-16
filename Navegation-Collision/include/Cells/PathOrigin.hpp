#ifndef PATH_ORIGIN_HPP
#define PATH_ORIGIN_HPP

#include "Cells/Cell.hpp"
#include "Math/Vector.hpp"
#include "Path/Manager.hpp"

namespace Cells {
class PathOrigin final : public ICell {
public:
  PathOrigin(Vec2u origin, Vec2u destination);

  void draw(Engine::Engine &engine) override;
  bool tick(Engine::Engine &engine, double dt) override;
  void clear() override;

private:
  Vec2u m_origin;
  Vec2u m_dest;
  Vec2u m_curr;

  Vec2 m_pos;

  PathManager::PathPtr m_path;
};
}; // namespace Cells

#endif // START_HPP
