#version 330 core

in vec2 Texcoord;

uniform sampler2D diffTex;

uniform float exposure;

out vec4 fragColor;

void main() {
	vec3 color = texture(diffTex, Texcoord).rgb;
	vec3 result = vec3(1.0) - exp(-color * exposure);

	const float gamma = 2.2;
	result = pow(result, vec3(1.0 / gamma));
	fragColor = vec4(result, 1.0);
	//fragColor = texture(diffTex, Texcoord);
}