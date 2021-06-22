#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(){
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	float specular = texture(diffuseTex, IN.texCoord).a;

	vec3 diff = texture(diffuseLight, IN.texCoord).xyz;
	vec3 spec = texture(specularLight, IN.texCoord).xyz;

	fragColour.xyz = diffuse * 0.1;//ambient
	fragColour.xyz += diffuse * diff;//diffuse
	fragColour.xyz += specular * spec;//specular
	fragColour.a = 1.0;
}