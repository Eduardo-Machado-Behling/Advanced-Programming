#ifndef SHADER_HPP
#define SHADER_HPP

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>

#include "engine_api.hpp"

namespace Engine {

struct ENGINE_API Shader {
  Shader();
  ~Shader();

  void bind();
  void unbind();

  bool add(GLenum type, const char *source);
  bool link();

private:
  std::vector<uint32_t> m_shaders;
  uint32_t m_id;
};

class ShaderManager {
public:
  void init(std::filesystem::path root);
  Shader &at(std::string_view name);

private:
  void load(std::string_view name);

  std::unordered_map<std::string, Shader> m_shaders;
  std::filesystem::path m_root;
};

} // namespace Engine

#endif
