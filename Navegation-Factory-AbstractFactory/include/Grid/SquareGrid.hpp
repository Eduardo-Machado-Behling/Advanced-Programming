#ifndef SQUARE_GRID_HPP
#define SQUARE_GRID_HPP

#include <Grid/Grid.hpp>

namespace Grid {
class SquareGrid : public IGrid {
public:
  SquareGrid(Vec2u coord);

  void draw(Engine::Engine &engine) override;

private:
};

} // namespace Grid

#endif // SQUARE_GRID_HPP