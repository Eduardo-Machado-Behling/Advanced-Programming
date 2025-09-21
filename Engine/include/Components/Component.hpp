#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <Utils/fixed_string.hpp>
#include <string_view>

namespace Engine {
namespace Components {

template <::Engine::Utils::fixed_string NAME> struct Component {
  inline static std::string_view name = NAME;

  virtual void bind();
  virtual void unbind();
};
} // namespace Components
} // namespace Engine

#endif
