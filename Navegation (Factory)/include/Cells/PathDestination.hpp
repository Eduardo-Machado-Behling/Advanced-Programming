#ifndef PATH_DESTINATION_HPP
#define PATH_DESTINATION_HPP

#include "Cells/Cell.hpp"

namespace Cells {
class PathDestination final : public ICell {
public:
  PathDestination(Vec2u destination, Vec2u origin);

  void draw(Engine::Engine &engine) override;
  bool tick(Engine::Engine &engine, double dt) override;
  void clear() override;

private:
  Vec2u m_dest;
  Vec2u m_origin;
};
}; // namespace Cells

#endif // PATHDESTINATION_HPP