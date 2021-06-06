#version 430 core
out vec4 FragColor;

in Vertex {
	vec3 fragPos;
	vec2 texcoords;
	mat3 TBN;
	vec3 viewPos;
} IN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform int numberOfTilesX;

struct PointLight{
    vec4 color;
    vec4 position_radius;
};

struct VisibleIndex {
	int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

#define NR_POINT_LIGHT 2000
#define MAX_NR_LIGHT 2048


vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos);

void main(){
	vec3 result=vec3(0.0);
	vec3 viewDir=normalize(IN.viewPos-IN.fragPos);
	vec3 n=vec3(texture(texture_normal1,IN.texcoords))*2.0-1.0;
	vec3 normal=normalize(IN.TBN*normalize(n));

	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * MAX_NR_LIGHT;
	for (uint i = 0; i < NR_POINT_LIGHT && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		PointLight light = lightBuffer.data[lightIndex];

		result+=calculatePointLight(light,viewDir,normal,IN.fragPos);
	}

	FragColor=vec4(result,1.0);
}

vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos){
	float dis=length(light.position_radius.xyz-fragPos);
	float atten=1.0-clamp(dis/light.position_radius.w,0.0,1.0);

	vec3 result=vec3(0.0);
	vec3 lightDir=normalize(light.position_radius.xyz-fragPos);
	float diff=max(dot(normal,lightDir),0.0);
	vec3 halfDir=normalize(viewDir+lightDir);
	float spec=pow(max(dot(halfDir,normal),0.0),32.0);

	vec3 ambient=light.color.rgb*vec3(texture(texture_diffuse1,IN.texcoords))*0.1;
	vec3 diffuse=light.color.rgb*vec3(texture(texture_diffuse1,IN.texcoords))*diff;
	vec3 specular=light.color.rgb*texture(texture_specular1,IN.texcoords).r*spec;

	ambient*=atten;
	diffuse*=atten;
	specular*=atten;

	result=ambient+diffuse+specular;

	return result;
}