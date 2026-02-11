#ifndef GRID_MANAGER_HPP
#define GRID_MANAGER_HPP

#include <memory>
#include <optional>
#include <unordered_set>

#include "Grid/Factories/Abstract.hpp"
#include "Grid/Graph.hpp"
#include "Grid/Grid.hpp"

#include "Observer/GridChanged.hpp"
#include "Observer/Publisher.hpp"

// Singleton
class GridManager {
public:
  struct AllocationParam {
    Vec2u gridConfig;
    Vec2 start;
    Vec2 end;
    int factoryIndex = -1;

    GridFactories::IFactory *gridFactory;
  };

  GridManager &clear();
  GridManager &allocate(AllocationParam param);
  GridManager &allocate(size_t rows, size_t cols, Vec2 start, Vec2 end,
                        GridFactories::IFactory *gridFactory = nullptr);

  GridManager &setGridFactory(GridFactories::IFactory *gridFactory);
  GridManager &resize(size_t rows, size_t cols);
  GridManager &deallocate();

  Grid::IGrid *get(Engine::Objects::ObjectUUID::UUID uuid);
  Grid::IGrid *get(size_t row, size_t col) const;
  Grid::IGrid *get(Vec2u coord) const;
  Grid::IGrid *get(Vec2 mouseCoord) const;

  const Grid::IGrid *cget(size_t row, size_t col) const;
  const Grid::IGrid *cget(Vec2u coord) const;
  const Grid::IGrid *cget(Vec2 mouseCoord) const;

  void setOccupied(Vec2u coord);
  void setFreed(Vec2u coord);

  Vec2u getCoord(Vec2 mouseCoord) const;
  Vec2 getCoord(Vec2u gridCoord) const;

  Vec2 getCenter(Vec2u gridCoord) const;
  Vec2 getCellSize();

  Grid::IGraph *getGraph();

  size_t rows() const;
  size_t cols() const;
  Vec2u resolution() const;

  Vec2 start() const;
  Vec2 end() const;

  bool allocated() const;

  void setup();
  bool update(Engine::Engine &engine, std::optional<double> dt = std::nullopt);

  GridManager &subscribeOnCellChange(Subscriber *obs);
  GridManager &unsubscribeOnCellChange(Subscriber *obs);
  GridManager &subscribeOnGridChange(Subscriber *obs);
  GridManager &unsubscribeOnGridChange(Subscriber *obs);

  static GridManager &get();

private:
  GridManager();

  Grid::IGrid *impl_get(size_t row, size_t col) const;
  inline size_t getGridIndex(size_t row, size_t col, Vec2u gridSize) const;
  inline Vec2u getIndexGrid(size_t index, Vec2u gridSize) const;

  static std::unique_ptr<GridManager> m_instance;
  std::unique_ptr<GridFactories::IFactory> m_factory;

  Subscribers::CallbackSubscriber m_changed;

  Publisher m_cellPublisher;
  Publisher m_gridPublisher;

  std::unordered_set<size_t> m_occupied;
  std::vector<std::unique_ptr<Grid::IGrid>> m_grid;

  std::unordered_map<Engine::Objects::ObjectUUID::UUID, Grid::IGrid *>
      m_uuidLookUp;
  std::unique_ptr<Grid::IGraph> m_graph;

  Vec2u m_gridSize;
  Vec2 m_area[2];
  Vec2 m_delta;
};

#endif // GRID_MANAGER_HPP
