#include "Cells/Factory.hpp"

#include "Cells/Decorators/PulsingEffect.hpp"
#include "Cells/Obstacle.hpp"
#include "Cells/PathDestination.hpp"
#include "Cells/PathOrigin.hpp"

Cells::ICell *CellFactory::Create(CellType type, Vec2u pos, Vec2u arg) const {
  Cells::ICell *newCell = nullptr;

  switch (type) {
  case CellType::DESTINATION:
    newCell = new Cells::PathDestination(pos, arg);
    break;

  case CellType::ORIGIN:
    newCell = new Cells::PathOrigin(pos, arg);
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
  if (!m_instance)
    m_instance.reset(new CellFactory());

  return *m_instance;
}

std::unique_ptr<CellFactory> CellFactory::m_instance = nullptr;