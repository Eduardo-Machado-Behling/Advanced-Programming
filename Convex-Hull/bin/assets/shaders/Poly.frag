#version 430 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out uint outUUID;

uniform uint UUID;
uniform vec3 borderColor;
uniform float borderSize;
uniform vec3 fillColor;

in vec3 barycentric;

void main(){
    // Find the smallest barycentric coordinate (closest distance to an edge)
    float minBary = min(min(barycentric.x, barycentric.y), barycentric.z);

    // fwidth() calculates how much the coordinate changes between this pixel
    // and the next. This makes the border a consistent pixel width.
    float edgeWidth = fwidth(minBary);

    // Create a smooth transition between the border and the fill color.
    // This produces a high-quality, anti-aliased line.
    float borderMix = smoothstep(borderSize, borderSize + edgeWidth, minBary);

    // If borderMix is 0, we are at the border. If it's 1, we are in the fill area.
    // mix() linearly interpolates between the two colors.
    vec4 finalColor = mix(vec4(borderColor, 1.0), vec4(fillColor, 1.0), borderMix);

	FragColor = finalColor;
	outUUID = UUID;
}

