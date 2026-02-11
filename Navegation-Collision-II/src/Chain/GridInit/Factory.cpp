#include "Chain/GridInit/Factory.hpp"

#include "Grid/Factories/Hexagonal.hpp"
#include "Grid/Factories/Square.hpp"

namespace Chain {

GridFactory::GridFactory() {}

std::function<GridFactories::IFactory *()> GridFactory::FACTORIES[] = {
    []() { return new GridFactories::SquareFactory(); },
    []() { return new GridFactories::HexagonalFactory(); },
};

bool GridFactory::process(GridManager::AllocationParam &param) {

  size_t factoriesAmount = sizeof(FACTORIES) / sizeof(*FACTORIES);

  if (param.factoryIndex < factoriesAmount) {
    param.gridFactory = FACTORIES[param.factoryIndex]();
    return true;
  }
  return false;
}
} // namespace Chain
