#version 430 core

layout (location = 0) in vec2 aPos;

layout(std140, binding = 0) uniform Matrices {
	mat4 mProj;
};

void main(){
	gl_Position = mProj * vec4(aPos, 0.0, 1.0);
}
