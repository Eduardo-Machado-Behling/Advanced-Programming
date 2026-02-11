#include "Observer/GridChanged.hpp"

namespace Subscribers {

void CallbackSubscriber::setOnChange(Callback callback) {
  m_onChanged = callback;
}

void CallbackSubscriber::update() {
  if (m_onChanged)
    m_onChanged();
}

} // namespace Subscribers
