#ifndef SQUARE_GRID_HPP
#define SQUARE_GRID_HPP

#include <Grid/Grid.hpp>

namespace Grid {
class SquareGrid : public IGrid {
public:
  SquareGrid(Vec2u coord);

  void draw(Engine::Engine &engine) override;
  Vec2 center() const override;
  float offsetRow(size_t row) const override;

private:
};

} // namespace Grid

#endif // SQUARE_GRID_HPP