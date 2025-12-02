#ifndef CELL_FACTORY_HPP
#define CELL_FACTORY_HPP

#include <memory>

#include "Cells/Cell.hpp"

// Singleton
class CellFactory {
public:
  enum class CellType {
    EMPTY,
    OBS,
    ORIGIN,
    DESTINATION,
  };

  Cells::ICell *Create(CellType type, Vec2u pos, Vec2u arg = {0, 0}) const;

  static const CellFactory &get();

private:
  CellFactory() = default;
  static std::unique_ptr<CellFactory> m_instance;
};

#endif // CELL_FACTORY_HPP