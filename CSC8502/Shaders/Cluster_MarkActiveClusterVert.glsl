#version 430 core

layout (location = 0 ) in vec3 aPos;
layout (location = 1 ) in vec2 aTexcoords;

out vec2 Texcoord;

void main() {
	gl_Position = vec4(aPos, 1.0);
	Texcoord = aTexcoords;
}