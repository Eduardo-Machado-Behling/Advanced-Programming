#ifndef GRID_MANAGER_HPP
#define GRID_MANAGER_HPP

#include <memory>
#include <optional>

#include "Grid/Factories/Abstract.hpp"
#include "Grid/Graph.hpp"
#include "Grid/Grid.hpp"

#include "Observer/GridChanged.hpp"
#include "Observer/Publisher.hpp"

// Singleton
class GridManager : public Publisher {
public:
  struct AllocationParam {
    Vec2u gridConfig;
    Vec2 start;
    Vec2 end;
    GridFactories::IFactory *gridFactory;
  };

  void clear();
  void allocate(AllocationParam param);
  void allocate(size_t rows, size_t cols, Vec2 start, Vec2 end,
                GridFactories::IFactory *gridFactory = nullptr);
  void deallocate();

  Grid::IGrid *get(size_t row, size_t col) const;
  Grid::IGrid *get(Vec2u coord) const;
  Grid::IGrid *get(Vec2 mouseCoord) const;

  const Grid::IGrid *cget(size_t row, size_t col) const;
  const Grid::IGrid *cget(Vec2u coord) const;
  const Grid::IGrid *cget(Vec2 mouseCoord) const;

  Vec2u getCoord(Vec2 mouseCoord) const;
  Vec2 getCoord(Vec2u gridCoord) const;

  Vec2 getCenter(Vec2u gridCoord) const;
  Vec2 getCellSize();

  Grid::IGraph *getGraph();

  size_t rows() const;
  size_t cols() const;

  Vec2 start() const;
  Vec2 end() const;

  bool allocated() const;

  bool update(Engine::Engine &engine,
              std::optional<double> dt = std::nullopt) const;

  static GridManager &get();

private:
  Grid::IGrid *impl_get(size_t row, size_t col) const;

  GridManager();
  static std::unique_ptr<GridManager> m_instance;
  std::unique_ptr<GridFactories::IFactory> m_factory;
  Subscribers::GridChanged m_changed;

  std::vector<std::unique_ptr<Grid::IGrid>> m_grid;
  std::unique_ptr<Grid::IGraph> m_graph;
  Vec2u m_gridSize;
  Vec2 m_area[2];
  Vec2 m_delta;
};

#endif // GRID_MANAGER_HPP
