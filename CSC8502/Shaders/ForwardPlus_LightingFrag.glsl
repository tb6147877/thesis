#version 430 core
out vec4 FragColor;

in Vertex {
	vec3 fragPos;
	vec2 texcoords;
	mat3 TBN;
	vec3 viewPos_tan;
	vec3 fragPos_tan;
} IN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform int numberOfTilesX;

struct PointLight{
    vec4 color;
    vec4 position;
    vec4 radius;
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

#define NR_POINT_LIGHT 6


vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos);

void main(){
	vec3 result=vec3(0.0);
	vec3 viewDir=normalize(IN.viewPos_tan-IN.fragPos_tan);
	vec3 n=vec3(texture(texture_normal1,IN.texcoords));
	vec3 normal=normalize(n*2.0-1.0);

	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * 1024;
	for (uint i = 0; i < NR_POINT_LIGHT && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		PointLight light = lightBuffer.data[lightIndex];

		result+=calculatePointLight(light,viewDir,normal,IN.fragPos_tan);
	}

	FragColor=vec4(result,1.0);
}

vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos){
	vec3 lightPos_tan=IN.TBN*light.position.xyz;
	float dis=length(lightPos_tan-fragPos);
	float atten=1.0-clamp(dis/light.radius.w,0.0,1.0);

	vec3 result=vec3(0.0);
	vec3 lightDir=normalize(lightPos_tan-fragPos);
	float diff=max(dot(normal,lightDir),0.0);
	vec3 halfDir=normalize(viewDir+lightDir);
	float spec=pow(max(dot(halfDir,normal),0.0),32.0);

	//if (diff == 0.0) {
		//spec = 0.0;
	//}

	vec3 ambient=light.color.rgb*vec3(texture(texture_diffuse1,IN.texcoords))*0.1;
	vec3 diffuse=light.color.rgb*vec3(texture(texture_diffuse1,IN.texcoords))*diff;
	vec3 specular=light.color.rgb*vec3(texture(texture_specular1,IN.texcoords))*spec;

	ambient*=atten;
	diffuse*=atten;
	specular*=atten;

	result=ambient+diffuse+specular;

	return result;
}