#include "Grid/Manager.hpp"

std::unique_ptr<GridManager> GridManager::m_instance = nullptr;

GridManager::GridManager() {
  m_changed.setOnChange([this]() { notifySubscribers(); });
}

void GridManager::clear() {
  for (auto &grid : m_grid) {
    grid->clear();
  }
}

void GridManager::allocate(AllocationParam param) {
  allocate(param.gridConfig[0], param.gridConfig[1], param.start, param.end,
           param.gridFactory);
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
      m_grid.back()->subscribe(&m_changed);
    }
  }
}

void GridManager::deallocate() { m_grid.clear(); }

Grid::IGrid *GridManager::impl_get(size_t row, size_t col) const {
  if (row >= m_gridSize[0] || col >= m_gridSize[1])
    return nullptr;
  return m_grid[row * m_gridSize[1] + col].get();
}

Grid::IGrid *GridManager::get(size_t row, size_t col) const {
  return impl_get(row, col);
}

Grid::IGrid *GridManager::get(Vec2u coord) const {
  return get(coord[1], coord[0]);
}
Grid::IGrid *GridManager::get(Vec2 mouseCoord) const {
  return get(getCoord(mouseCoord));
}

const Grid::IGrid *GridManager::cget(size_t row, size_t col) const {
  return impl_get(row, col);
}
const Grid::IGrid *GridManager::cget(Vec2u coord) const {
  return cget(coord[1], coord[0]);
}
const Grid::IGrid *GridManager::cget(Vec2 mouseCoord) const {
  return cget(getCoord(mouseCoord));
}

Vec2u GridManager::getCoord(Vec2 mouseCoord) const {
  return m_factory->getGridCoord(mouseCoord, m_area[0], m_delta, m_gridSize[0],
                                 m_gridSize[1]);
}

Vec2 GridManager::getCoord(Vec2u gridCoord) const {
  Vec2 mousePos = m_area[0];
  mousePos += {m_delta[0] * gridCoord[0], m_delta[1] * gridCoord[1]};

  return mousePos;
}

Vec2 GridManager::getCenter(Vec2u gridCoord) const {
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
    if (dt) {
      if (!grid->tick(engine, *dt)) {
        allDone = false;
      }
    }
    grid->draw(engine);
  }
  return allDone;
}

  const Publisher& GridManager::getCellChanged(){
	  return m_gridChanged;
  }

  const Publisher& GridManager::getGridChanged(){
	
  }

GridManager &GridManager::get() {
  if (!m_instance)
    m_instance.reset(new GridManager());
  return *m_instance;
}
