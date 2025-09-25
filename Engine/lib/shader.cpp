#include "shader.hpp"
#include "Math/Vector.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace Engine {

Shader::Shader() { m_id = glCreateProgram(); }
Shader::~Shader() { glDeleteProgram(m_id); }

template <> void Shader::set<uint32_t>(const char *name, uint32_t val) {
  uint32_t loc = glGetUniformLocation(m_id, name);
  if (loc == -1) {
    std::cout << "Unifrom \"" << name << "\" not found\n";
    return;
  }

  glUniform1ui(loc, val);
}

template <>
void Shader::set<Math::Vector<3>>(const char *name, Math::Vector<3> val) {
  uint32_t loc = glGetUniformLocation(m_id, name);
  if (loc == -1) {
    std::cout << "Unifrom \"" << name << "\" not found\n";
    return;
  }

  glUniform3fv(loc, 1, &val[0]);
}
void Shader::bind() { glUseProgram(m_id); }
void Shader::unbind() { glUseProgram(0); }

bool Shader::add(GLenum type, const char *src) {
  GLuint id = glCreateShader(type);

  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cerr << "Failed to compile "
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << " shader!" << std::endl;
    std::cerr << message << std::endl;
    glDeleteShader(id);
    return false;
  }

  m_shaders.push_back(id);
  return true;
}

bool Shader::link() {
  for (auto id : m_shaders) {
    glAttachShader(m_id, id);
  }

  glLinkProgram(m_id);
  glValidateProgram(m_id);

  // Error checking for linking
  GLint linkStatus;
  glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    GLint length;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetProgramInfoLog(m_id, length, &length, message);
    std::cerr << "Failed to link program!" << std::endl;
    std::cerr << message << std::endl;
    return false;
  }

  return true;
}

void ShaderManager::init(std::filesystem::path root) { m_root = root; }

Shader &ShaderManager::at(std::string_view name) {
  std::string key(name);

  if (!m_shaders.contains(key)) {
    load(key);
  }

  return m_shaders.at(key);
}

void ShaderManager::load(std::string_view name) {
  const char EXTS[][6] = {".vert", ".frag", ".geo", ".tes", ".tcs", ".comp"};

  const GLenum EXTS_ENUM[] = {
      GL_VERTEX_SHADER,          GL_FRAGMENT_SHADER,     GL_GEOMETRY_SHADER,
      GL_TESS_EVALUATION_SHADER, GL_TESS_CONTROL_SHADER, GL_COMPUTE_SHADER,
  };

  std::string strName = std::string(name);
  Shader &shader = m_shaders[strName];
  std::cout << "Loading: " << name << '\n';
  for (size_t i = 0; i < (sizeof(EXTS) / sizeof(EXTS[0])); i++) {
    std::filesystem::path file = m_root / (strName + EXTS[i]);

    std::stringstream source;
    std::cout << "\tSearching: " << file;
    if (std::filesystem::exists(file)) {
      std::cout << " Found\n";
      std::ifstream sourceFile(file);
      source << sourceFile.rdbuf();

      std::cout << "Shader added: " << file.string() << '\n';
      shader.add(EXTS_ENUM[i], source.str().c_str());
    } else {
      std::cout << " Not Found\n";
    }
  }

  shader.link();
}

} // namespace Engine
