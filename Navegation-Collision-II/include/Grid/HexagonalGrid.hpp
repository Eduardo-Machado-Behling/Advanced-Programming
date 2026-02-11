#ifndef HEXAGONAL_GRID_HPP
#define HEXAGONAL_GRID_HPP

#include <Grid/Grid.hpp>

namespace Grid {
class HexagonalGrid : public IGrid {
public:
  HexagonalGrid(Vec2u coord);

  void draw(Engine::Engine &engine) override;
  Vec2 center() const override;
  Engine::Objects::ObjectUUID::UUID getUUID() const override;
  float offsetRow(size_t row) const override;

private:
  Engine::Poly *poly;
};
} // namespace Grid

#endif // HEXAGONAL_GRID_HPP