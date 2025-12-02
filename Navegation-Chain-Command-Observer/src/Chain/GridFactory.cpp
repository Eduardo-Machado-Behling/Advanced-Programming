#include "Chain/GridFactory.hpp"

#include "Grid/Factories/Hexagonal.hpp"
#include "Grid/Factories/Square.hpp"

namespace Chain {
bool GridFactory::process(GridManager::AllocationParam &param) {
  int type = 1;
  std::cout << "type (1=Square, 2=Hex): ";
  std::cin >> type;

  switch (type) {
  case 2:
    param.gridFactory = new GridFactories::HexagonalFactory();
    break;
  case 1:
    param.gridFactory = new GridFactories::SquareFactory();
    break;

  default:
    return false;
  }

  return true;
}
} // namespace Chain
