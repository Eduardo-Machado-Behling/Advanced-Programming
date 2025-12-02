#include "Chain/GridConfig.hpp"

namespace Chain {
	bool GridConfig::process(GridManager::AllocationParam &param) {
		std::cout << "Rows: ";
		std::cin >> param.gridConfig[0];
		std::cout << "Cols: ";
		std::cin >> param.gridConfig[1];

		return true;
	}
}
