#version 430 core

flat in vec3 color;
flat in uint UUID;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out uint outUUID;

void main(){
	FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	outUUID = UUID;
}
