#include "Cells/Factory.hpp"

#include "Cells/Obstacle.hpp"
#include "Cells/PathDestination.hpp"
#include "Cells/PathOrigin.hpp"

Cells::ICell *CellFactory::Create(CellType type, Vec2u pos, Vec2u arg) const {
  switch (type) {
  case CellType::DESTINATION:
    return new Cells::PathDestination(pos, arg);
    break;

  case CellType::ORIGIN:
    return new Cells::PathOrigin(pos, arg);
    break;

  case CellType::OBS:
    return new Cells::Obstacle(pos);
    break;

  default:
    throw new std::invalid_argument("Unkown Cell type");
    break;
  }
}

const CellFactory &CellFactory::get() {
  if (!m_instance)
    m_instance.reset(new CellFactory());

  return *m_instance;
}

std::unique_ptr<CellFactory> CellFactory::m_instance = nullptr;