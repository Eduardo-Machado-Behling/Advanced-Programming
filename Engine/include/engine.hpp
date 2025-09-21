#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>
#include <glad/glad.h>

namespace Engine {
// Singleton
class Engine{
public:
	static Engine* init(GLADloadproc proc);
	static Engine* get();

	void update(double dt);
	void draw();
	
private:
	Engine();

// Variables

public:
private:
	inline static std::unique_ptr<Engine> m_instance = nullptr;
};
	
} // Engine

#endif
