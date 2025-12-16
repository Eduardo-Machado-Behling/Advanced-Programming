#include "Chain/GridInit/Config.hpp"

namespace Chain {
GridConfig::GridConfig(bool skip) : m_ask(skip) {}
bool GridConfig::process(GridManager::AllocationParam &param) {
  if (m_ask) {
    std::cout << "Rows: ";
    std::cin >> param.gridConfig[0];
    std::cout << "Cols: ";
    std::cin >> param.gridConfig[1];
  } else {
    param.gridConfig[0] = 10;
    param.gridConfig[1] = 10;
  }

  return param.gridConfig[0] > 0 && param.gridConfig[1] > 0;
}
} // namespace Chain
