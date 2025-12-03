#version 430 core

layout(location = 0) in vec2 aPos;

layout(location = 1) in uint aUUID;
layout(location = 2) in vec3 aColor;
layout(location = 3) in mat4 aModel;

layout(std140, binding = 0) uniform Matrices { mat4 mProj; };

flat out vec3 color;
flat out uint UUID;

void main() {
  gl_Position = mProj * aModel * vec4(aPos, 1.0, 1.0);

  color = aColor;
  UUID = aUUID;
}
