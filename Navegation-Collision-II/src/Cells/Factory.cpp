#include "Cells/Factory.hpp"

#include "Cells/Decorators/PulsingEffect.hpp"
#include "Cells/Obstacle.hpp"
#include "Cells/PathDestination.hpp"
#include "Cells/PathOrigin.hpp"

Cells::ICell *CellFactory::Create(CellType type, Vec2u pos,
                                  std::optional<Vec2u> arg) const {
  Cells::ICell *newCell = nullptr;

  switch (type) {
  case CellType::DESTINATION:
    if (arg)
      newCell = new Cells::PathDestination(pos, arg.value());
    break;

  case CellType::ORIGIN:
    if (arg)
      newCell = new Cells::PathOrigin(pos, arg.value());
    else
      newCell = new Cells::PathOrigin(pos);
    break;

  case CellType::OBS:
    newCell = new Cells::Obstacle(pos);
    break;

  default:
    throw new std::invalid_argument("Unknown Cell type");
    break;
  }
  if (type == CellType::OBS) {
    return new Cells::PulsingDecorator(newCell);
  }

  return newCell;
}

const CellFactory &CellFactory::get() {
  static CellFactory *m_instance = new CellFactory();

  return *m_instance;
}
