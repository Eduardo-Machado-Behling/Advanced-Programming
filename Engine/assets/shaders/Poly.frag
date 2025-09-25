#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out uint outUUID;

uniform uint UUID;
uniform vec3 borderColor;
uniform float borderSize;
uniform vec3 fillColor;

void main(){
	FragColor = vec4(fillColor, 1.0);
	outUUID = UUID;
}

