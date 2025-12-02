#ifndef CELL_DECORATOR_HPP
#define CELL_DECORATOR_HPP

#include "Cells/Cell.hpp"
#include <memory>

namespace Cells {

class CellDecorator : public ICell {
protected:
  std::unique_ptr<ICell> m_wrappedCell;

public:
  CellDecorator(ICell *cell) : m_wrappedCell(cell) {}

  virtual ~CellDecorator() = default;

  void draw(Engine::Engine &engine) override {
    if (m_wrappedCell)
      m_wrappedCell->draw(engine);
  }

  bool tick(Engine::Engine &engine, double dt) override {
    if (m_wrappedCell)
      return m_wrappedCell->tick(engine, dt);
    return true;
  }

  void clear() override {
    if (m_wrappedCell)
      m_wrappedCell->clear();
  }
};

} // namespace Cells

#endif // CELL_DECORATOR_HPP