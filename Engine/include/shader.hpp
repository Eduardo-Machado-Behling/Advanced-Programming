#ifndef SHADER_HPP
#define SHADER_HPP

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Engine {

struct Shader {
  void bind();
  void unbind();

private:
  uint32_t id;
};

class ShaderManager {
public:
  static ShaderManager *get();

  void init(std::filesystem::path root);
  void at(std::string_view name);

private:
  std::unordered_map<std::string, Shader> m_shaders;
  std::unique_ptr<ShaderManager> m_instance;
};

} // namespace Engine

#endif
