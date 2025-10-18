#version 430 core

flat in vec3 color;
flat in uint UUID;

flat in float radius;
flat in vec2 center;
flat in mat4 invProj;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out uint outUUID;

void main(){
	vec2 dist = (gl_FragCoord.xy - center);
	float magDist = dist.x * dist.x + dist.y * dist.y;
	if(magDist > radius)
		discard;

	FragColor = vec4(color, 1.0);
	outUUID = UUID;
}
