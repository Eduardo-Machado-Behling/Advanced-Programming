#include "Chain/GridManager.hpp"
#include "Grid/Manager.hpp"

namespace Chain {

bool GridManager::process(::GridManager::AllocationParam &param) {
  ::GridManager::get().allocate(param);

  return ::GridManager::get().allocated();
}

} // namespace Chain
