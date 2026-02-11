#ifndef CELL_FACTORY_HPP
#define CELL_FACTORY_HPP

#include <memory>
#include <optional>

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

  Cells::ICell *Create(CellType type, Vec2u pos,
                       std::optional<Vec2u> arg = std::nullopt) const;

  static const CellFactory &get();

private:
  CellFactory() = default;
  static std::unique_ptr<CellFactory> m_instance;
};

#endif // CELL_FACTORY_HPP