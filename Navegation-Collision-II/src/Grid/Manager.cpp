#include "Grid/Manager.hpp"

GridManager::GridManager() {
  m_changed.setOnChange([this]() { m_cellPublisher.notifySubscribers(); });
}

GridManager &GridManager::clear() {
  for (auto &grid : m_grid) {
    grid->clear();
  }

  return *this;
}

GridManager &GridManager::allocate(AllocationParam param) {
  return allocate(param.gridConfig[0], param.gridConfig[1], param.start,
                  param.end, param.gridFactory);
}

GridManager &GridManager::allocate(size_t rows, size_t cols, Vec2 start,
                                   Vec2 end,
                                   GridFactories::IFactory *gridFactory) {
  m_area[0] = start;
  m_area[1] = end;

  m_gridSize[0] = rows;
  m_gridSize[1] = cols;

  m_grid.reserve(rows * cols);

  if (gridFactory) {
    m_factory.reset(gridFactory);
  }

  m_graph.reset(m_factory->createGraph());

  Vec2 areaSize = m_area[1] - m_area[0];
  m_delta = m_factory->calculateDelta(areaSize, rows, cols);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      m_grid.emplace_back(m_factory->createGrid({i, j}));
      m_grid.back()->subscribeOnChanged(&m_changed);
    }
  }

  m_gridPublisher.notifySubscribers();

  return *this;
}

GridManager &GridManager::setGridFactory(GridFactories::IFactory *gridFactory) {
  if (gridFactory) {
    m_factory.reset(gridFactory);
  }

  m_graph.reset(m_factory->createGraph());

  for (size_t i = 0; i < m_gridSize[1]; i++) {
    for (size_t j = 0; j < m_gridSize[0]; j++) {
      size_t idx = i * m_gridSize[1] + j;

      auto *grid = m_factory->createGrid({i, j});
      grid->set(m_grid[idx]->reset());
      grid->subscribeOnChanged(&m_changed);
      grid->tickSetup();

      m_grid[idx].reset(grid);
    }
  }

  return *this;
}

GridManager &GridManager::resize(size_t rows, size_t cols) {
  Vec2u oldSize = m_gridSize;
  m_gridSize = {rows, cols};
  size_t newTotal = rows * cols;

  Vec2 areaSize = m_area[1] - m_area[0];
  m_delta = m_factory->calculateDelta(areaSize, rows, cols);

  std::vector<std::unique_ptr<Grid::IGrid>> newGrid;
  newGrid.reserve(newTotal);

  for (size_t r = 0; r < rows; ++r) {
    for (size_t c = 0; c < cols; ++c) {
      if (r < oldSize[0] && c < oldSize[1]) {
        size_t oldIdx = r * oldSize[1] + c;
        newGrid.push_back(std::move(m_grid.at(oldIdx)));
      } else {
        Vec2u pos = {r, c};
        auto item = m_factory->createGrid(pos);
        item->subscribeOnChanged(&m_changed);
        newGrid.emplace_back(item);
      }
    }
  }

  m_grid = std::move(newGrid);

  for (auto &grid : m_grid) {
    grid->tickSetup();
  }

  std::unordered_set<size_t> newOccupied;
  for (size_t o : m_occupied) {
    Vec2u pos = getIndexGrid(o, oldSize);
    if (pos[0] < rows && pos[1] < cols) {
      newOccupied.insert(pos[0] * cols + pos[1]);
    }
  }
  m_occupied = std::move(newOccupied);

  m_gridPublisher.notifySubscribers();

  return *this;
}

GridManager &GridManager::deallocate() {
  m_grid.clear();
  m_gridPublisher.notifySubscribers();
  return *this;
}

Grid::IGrid *GridManager::impl_get(size_t row, size_t col) const {
  if (row >= m_gridSize[0] || col >= m_gridSize[1])
    return nullptr;
  return m_grid[row * m_gridSize[1] + col].get();
}

inline size_t GridManager::getGridIndex(size_t row, size_t col,
                                        Vec2u gridSize) const {
  return row * gridSize[1] + col;
}

inline Vec2u GridManager::getIndexGrid(size_t index, Vec2u gridSize) const {
  return {index % gridSize[1], index / gridSize[1]};
}

Grid::IGrid *GridManager::get(Engine::Objects::ObjectUUID::UUID uuid) {
  if (!m_uuidLookUp.contains(uuid))
    return nullptr;

  return m_uuidLookUp[uuid];
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

void GridManager::setOccupied(Vec2u coord) {
  m_occupied.insert(getGridIndex(coord[1], coord[0], m_gridSize));
}

void GridManager::setFreed(Vec2u coord) {
  m_occupied.erase(getGridIndex(coord[1], coord[0], m_gridSize));
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

Vec2u GridManager::resolution() const { return m_gridSize; }
size_t GridManager::rows() const { return m_gridSize[0]; }
size_t GridManager::cols() const { return m_gridSize[1]; }

Vec2 GridManager::start() const { return m_area[0]; }
Vec2 GridManager::end() const { return m_area[1]; }

bool GridManager::allocated() const { return !m_grid.empty(); }

void GridManager::setup() {
  for (auto &grid : m_grid) {
    grid->tickSetup();
  }
}
bool GridManager::update(Engine::Engine &engine, std::optional<double> dt) {
  bool allDone = true;
  m_uuidLookUp.clear();

  for (auto &grid : m_grid) {
    if (dt) {
      if (!grid->tick(engine, *dt)) {
        allDone = false;
      }
    }
    grid->draw(engine);
    m_uuidLookUp.insert({grid->getUUID(), grid.get()});
  }
  return allDone;
}

GridManager &GridManager::subscribeOnCellChange(Subscriber *obs) {
  m_cellPublisher.subscribe(obs);
  return *this;
}
GridManager &GridManager::unsubscribeOnCellChange(Subscriber *obs) {
  m_cellPublisher.unsubscribe(obs);
  return *this;
}

GridManager &GridManager::subscribeOnGridChange(Subscriber *obs) {
  m_gridPublisher.subscribe(obs);
  return *this;
}

GridManager &GridManager::unsubscribeOnGridChange(Subscriber *obs) {
  m_gridPublisher.unsubscribe(obs);
  return *this;
}

GridManager &GridManager::get() {
  static GridManager *m_instance = new GridManager();
  return *m_instance;
}
