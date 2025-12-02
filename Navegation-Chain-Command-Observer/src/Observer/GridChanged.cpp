#include "Observer/GridChanged.hpp"

namespace Subscribers {

void GridChanged::setOnChange(Callback callback) { m_onChanged = callback; }

void GridChanged::update() {
  if (m_onChanged)
    m_onChanged();
}

} // namespace Subscribers
