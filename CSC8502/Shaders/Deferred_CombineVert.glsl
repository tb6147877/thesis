#version 330 core

layout (location = 0 ) in vec3 aPos;
layout (location = 1 ) in vec2 aTexcoords;

out Vertex {
	vec2 texCoord ;
} OUT;

void main() {
	gl_Position = vec4(aPos, 1.0);
	OUT.texCoord = aTexcoords;
}