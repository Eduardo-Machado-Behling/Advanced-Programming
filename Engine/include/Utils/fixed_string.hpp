#include <algorithm>
#include <string_view>

namespace Engine {

namespace Utils {
template <size_t N> struct fixed_string {
  char data[N]{};

  constexpr fixed_string(const char (&str)[N]) { std::copy_n(str, N, data); }
  constexpr operator std::string_view() const { return {data, N - 1}; }
};
} // namespace Utils
} // namespace Engine
