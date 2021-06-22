#version 430 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float lightRadius;
uniform vec3 lightPos;

layout (location = 0) in vec3 position;

/*
struct PointLight{
    vec4 color;
    vec4 position_radius;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;


out Vertex {
	vec3 color;
    vec3 position;
  float radius;
} OUT ;
*/

void main(){
	vec3 scale = vec3(lightRadius);
 	vec3 worldPos = (position * scale) + lightPos;
 	gl_Position = (projMatrix * viewMatrix) * vec4(worldPos, 1.0);

 	//use drawinstanced optimize
 	/*
 	PointLight temp=lightBuffer.data[gl_InstanceID];
 	vec3 scale = vec3(temp.position_radius.a);
 	vec3 worldPos = (position * scale) + temp.position_radius.xyz;
 	gl_Position = (projMatrix * viewMatrix) * vec4(worldPos, 1.0);

 	OUT.color=temp.color.xyz;
 	OUT.position=temp.position_radius.xyz;
 	OUT.radius=temp.position_radius.w;
 	*/
}