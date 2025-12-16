#include "Chain/GridInit/Factory.hpp"

#include "Grid/Factories/Hexagonal.hpp"
#include "Grid/Factories/Square.hpp"

namespace Chain {

GridFactory::GridFactory(bool ask) : m_ask(ask) {}
bool GridFactory::process(GridManager::AllocationParam &param) {
  int type = 1;
  if (m_ask) {
    std::cout << "type (1=Square, 2=Hex): ";
    std::cin >> type;
  }

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
