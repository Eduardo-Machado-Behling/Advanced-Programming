#version 430 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint outUUID;

uniform uint UUID;
uniform vec3 borderColor;
uniform float borderSize;
uniform vec3 fillColor;

in vec3 barycentric;

void main() {
  float minBary = min(min(barycentric.x, barycentric.y), barycentric.z);

  float edgeWidth = fwidth(minBary);

  float borderMix = smoothstep(borderSize, borderSize + edgeWidth, minBary);

  vec4 finalColor =
      mix(vec4(borderColor, 1.0), vec4(fillColor, 1.0), borderMix);

  FragColor = finalColor;
  outUUID = UUID;
}
