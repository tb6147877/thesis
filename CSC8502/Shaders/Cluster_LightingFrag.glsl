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

struct PointLight{
    vec4 color;
    vec4 position_radius;
};

struct LightGrid{
    uint offset;
    uint count;
};

layout (std430, binding = 2) buffer ClusterBasicData{
	mat4 inverseProj;
    uvec4 clusterSizes;
    uvec2 screenSizes;
    float scale;
    float bias;
};

layout(std430, binding = 3) buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout (std430, binding = 4) buffer LightIndexSSBO{
    uint data[];
} lightIndexList;

layout (std430, binding = 5) buffer LightGridSSBO{
    LightGrid data[];
} lightGrids;

uniform float zFar;
uniform float zNear;
uniform bool showSlice;

float linearDepth(float depth);
vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos);

vec3 sliceColors[8] = vec3[](
   vec3(0, 0, 0),    vec3( 0,  0,  1), vec3( 0, 1, 0),  vec3(0, 1,  1),
   vec3(1,  0,  0),  vec3( 1,  0,  1), vec3( 1, 1, 0),  vec3(1, 1, 1)
);



void main(){

    vec3 result=vec3(0.0);
	vec3 viewDir=normalize(IN.viewPos-IN.fragPos);
	vec3 n=vec3(texture(texture_normal1,IN.texcoords))*2.0-1.0;
	vec3 normal=normalize(IN.TBN*normalize(n));

    uint slice = uint(max(log2(linearDepth(gl_FragCoord.z)) * scale + bias, 0.0));
    uvec3 cluster = uvec3(uvec2( gl_FragCoord.xy / clusterSizes[3] ), slice);
    uint clusterIndex = cluster.x + clusterSizes.x * cluster.y + (clusterSizes.x * clusterSizes.y) * cluster.z;

    uint lightCount = lightGrids.data[clusterIndex].count;
    uint lightIndexOffset = lightGrids.data[clusterIndex].offset;

    for(uint i = 0; i < lightCount; i++){
        uint lightIndex = lightIndexList.data[lightIndexOffset + i];
        PointLight light = lightBuffer.data[lightIndex];
        result+=calculatePointLight(light, viewDir, normal, IN.fragPos);
    }

    if(showSlice){
        FragColor = vec4(sliceColors[uint(mod(float(slice), 8.0))], 1.0);
    }else{
        FragColor=vec4(result,1.0);
    }
    
}



float linearDepth(float depth){
    float depthRange = 2.0 * depth - 1.0;
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
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