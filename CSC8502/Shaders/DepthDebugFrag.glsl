#version 330 core

uniform float near;
uniform float far;

out vec4 fragColor;

// Need to linearize the depth because we are using the projection
float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	//this is a formula which can linearize depth value
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	fragColor = vec4(vec3(depth), 1.0f);
}