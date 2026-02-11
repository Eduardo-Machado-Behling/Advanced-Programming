#ifndef SQUARE_GRID_HPP
#define SQUARE_GRID_HPP

#include <Grid/Grid.hpp>
#include <engine.hpp>

namespace Grid {
class SquareGrid : public IGrid {
public:
  SquareGrid(Vec2u coord);

  void draw(Engine::Engine &engine) override;
  Vec2 center() const override;
  Engine::Objects::ObjectUUID::UUID getUUID() const override;
  float offsetRow(size_t row) const override;

private:
  Engine::Poly *poly;
};

} // namespace Grid

#endif // SQUARE_GRID_HPP