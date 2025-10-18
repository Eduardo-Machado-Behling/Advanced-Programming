#version 430 core

layout (location = 0) in vec2 aPos;

out vec3 barycentric;

layout(std140, binding = 0) uniform Matrices {
	mat4 mProj;
};

void main(){
	gl_Position = mProj * vec4(aPos, 0.0, 1.0);

    if (gl_VertexID % 3 == 0) {
        barycentric = vec3(1, 0, 0);
    } else if (gl_VertexID % 3 == 1) {
        barycentric = vec3(0, 1, 0);
    } else {
        barycentric = vec3(0, 0, 1);
    }
}
