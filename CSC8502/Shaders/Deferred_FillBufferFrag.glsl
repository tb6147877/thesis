#version 330 core

layout (location = 0) out vec4 diffTex;
layout (location = 1) out vec4 normTex;


in Vertex {
	vec2 texcoords;
	mat3 TBN;
} IN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;



void main(){

	diffTex=texture(texture_diffuse1,IN.texcoords);
	diffTex.a=texture(texture_specular1,IN.texcoords).r;

	vec3 normal = texture (texture_normal1,IN.texcoords).rgb * 2.0 - 1.0;
 	normal = normalize(IN.TBN * normalize(normal));
 	normTex=vec4(normal.xyz*0.5+0.5,1.0);
}