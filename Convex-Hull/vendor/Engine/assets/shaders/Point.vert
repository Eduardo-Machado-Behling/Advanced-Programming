#version 430 core

layout (location = 0) in vec2 aPos;

layout (location = 1) in uint aUUID;
layout (location = 2) in vec3 aColor;
layout (location = 3) in mat4 aModel;

layout(std140, binding = 0) uniform Matrices {
	mat4 mProj;
};

flat out vec3 color;
flat out uint UUID;

flat out float radius;
flat out mat4 invProj;
flat out vec2 center;

void main(){
	gl_Position = mProj * aModel * vec4(aPos, 0.0, 1.0);
	center = (aModel * vec4(0.5f, 0.5f, 0.0, 1.0)).xy;

	float rad = aModel[0][0] * 0.5f;
	radius = rad * rad;
	color = aColor;
	invProj = inverse(mProj);
	UUID = aUUID;

}
