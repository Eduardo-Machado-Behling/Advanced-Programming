#include "engine.hpp"

#include <GL/gl.h>
#include <glad/glad.h>

namespace Engine {
Engine* Engine::init(GLADloadproc proc){
    if (gladLoadGLLoader(proc)) {
		m_instance.reset(new Engine());
    }

	return m_instance->get();
}

Engine* Engine::get() {
  return m_instance->get();
}

void Engine::draw() {
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace Engine
