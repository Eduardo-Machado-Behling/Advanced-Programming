#ifndef GRID_ABSTRACT_FACTORY_HPP
#define GRID_ABSTRACT_FACTORY_HPP

#include "Grid/Grid.hpp"

namespace GridFactories {
struct IFactory {
  virtual ~IFactory() = default;

  virtual Grid::IGrid *createGrid(Vec2u pos) const = 0;
};
} // namespace GridFactories

#endif // GRID_ABSTRACT_FACTORY_HPP