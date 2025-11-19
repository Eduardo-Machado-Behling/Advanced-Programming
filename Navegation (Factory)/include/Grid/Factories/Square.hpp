#ifndef SQUARE_GRID_FACTORY_HPP
#define SQUARE_GRID_FACTORY_HPP

#include "Grid/Factories/Abstract.hpp"

namespace GridFactories {
class SquareFactory : public IFactory {
public:
  Grid::IGrid *createGrid(Vec2u pos) const override;
};

} // namespace GridFactories

#endif // SQUARE_GRID_FACTORY_HPP