#include "Grid/Manager.hpp"

std::unique_ptr<GridManager> GridManager::m_instance = nullptr;

void GridManager::clear() {
  for (auto &grid : m_grid) {
    grid->clear();
  }
}

void GridManager::allocate(AllocationParam param){
	allocate(param.gridConfig[0], param.gridConfig[1], param.start, param.end, param.gridFactory);
}

void GridManager::allocate(size_t rows, size_t cols, Vec2 start, Vec2 end,
                           GridFactories::IFactory *gridFactory) {
  m_area[0] = start;
  m_area[1] = end;

  m_gridSize[0] = rows;
  m_gridSize[1] = cols;

  m_grid.reserve(rows * cols);

  if (gridFactory) {
    m_factory.reset(gridFactory);
  }

  Vec2 areaSize = m_area[1] - m_area[0];
  m_delta = m_factory->calculateDelta(areaSize, rows, cols);

  m_graph.reset(m_factory->createGraph());
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      m_grid.emplace_back(m_factory->createGrid({i, j}));
    }
  }
}

void GridManager::deallocate() { m_grid.clear(); }

Grid::IGrid *GridManager::get(size_t row, size_t col) {
  if (row >= m_gridSize[0] || col >= m_gridSize[1])
    return nullptr;
  return m_grid[row * m_gridSize[1] + col].get();
}

Grid::IGrid *GridManager::get(Vec2u coord) { 
	notifySubscribers();
	return get(coord[1], coord[0]);
}
Grid::IGrid *GridManager::get(Vec2 mouseCoord) {
  return get(getCoord(mouseCoord));
}

const Grid::IGrid *GridManager::cget(size_t row, size_t col) {
  return get(row, col);
}
const Grid::IGrid *GridManager::cget(Vec2u coord) { return get(coord); }
const Grid::IGrid *GridManager::cget(Vec2 mouseCoord) {
  return get(mouseCoord);
}

Vec2u GridManager::getCoord(Vec2 mouseCoord) {
  return m_factory->getGridCoord(mouseCoord, m_area[0], m_delta, m_gridSize[0],
                                 m_gridSize[1]);
}

Vec2 GridManager::getCoord(Vec2u gridCoord) {
  Vec2 mousePos = m_area[0];
  mousePos += {m_delta[0] * gridCoord[0], m_delta[1] * gridCoord[1]};

  return mousePos;
}

Vec2 GridManager::getCenter(Vec2u gridCoord) {
  return cget(gridCoord)->center();
}

Vec2 GridManager::getCellSize() { return m_delta; }

Grid::IGraph *GridManager::getGraph() { return m_graph.get(); }

size_t GridManager::rows() const { return m_gridSize[0]; }
size_t GridManager::cols() const { return m_gridSize[1]; }

Vec2 GridManager::start() const { return m_area[0]; }
Vec2 GridManager::end() const { return m_area[1]; }

bool GridManager::allocated() const { return !m_grid.empty(); }

bool GridManager::update(Engine::Engine &engine,
                         std::optional<double> dt) const {
  bool allDone = true;
  for (auto &grid : m_grid) {
    grid->draw(engine);
    if (dt) {
      if (!grid->tick(engine, *dt)) {
        allDone = false;
      }
    }
  }
  return allDone;
}

GridManager &GridManager::get() {
  if (!m_instance)
    m_instance.reset(new GridManager());
  return *m_instance;
}
