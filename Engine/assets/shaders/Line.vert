#version 430 core

layout (location = 0) in vec2 aPos;

layout (location = 1) in vec3 aColor;
layout (location = 2) in mat4 aModel;

layout(std140, binding = 0) uniform Matrices {
	mat4 mProj;
};

flat out vec3 color;

void main(){
	gl_Position = mProj * aModel * vec4(aPos, 0.0, 1.0);

	color = aColor;
}

