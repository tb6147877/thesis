#version 430 core

layout(local_size_x = 16, local_size_y = 9, local_size_z = 4) in;

struct PointLight {
	vec4 color;
	vec4 position_radius;
};

struct LightGrid{
    uint offset;
    uint count;
};

struct ClusterAABBVolume{
    vec4 minPoint;
    vec4 maxPoint;
};

layout (std430, binding=1) buffer ClusterAABB{
	ClusterAABBVolume data[];
} clusterAABBs;

layout (std430, binding=2) readonly buffer ClusterBasicData{
	mat4 inverseProj;
    uvec4 clusterSizes;
    uvec2 screenSizes;
};

layout(std430, binding = 3) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout (std430, binding = 4) writeonly buffer LightIndexSSBO{
    uint data[];
} lightIndexList;

layout (std430, binding = 5) writeonly buffer LightGridSSBO{
    LightGrid data[];
} lightGrids;

layout (std430, binding = 6) buffer GlobalIndexCountSSBO{
    uint globalIndexCount;
};

shared PointLight sharedLights[16*9*4];

uniform mat4 viewMatrix;

bool detectSphereAABB(uint light, uint cluster);
float squaredDistancePointAndAABB(vec3 point, uint cluster);

void main(){
    globalIndexCount=0;
    uint threadCount = gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z;
    uint lightCount  = lightBuffer.data.length();
    uint batchNumbers = (lightCount + threadCount -1) / threadCount;

    uint clusterIndex = gl_LocalInvocationIndex + gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z * gl_WorkGroupID.z;
    uint visibleLightCount = 0;
    uint visibleLightIndices[100];

    for(int i=0;i<batchNumbers;i++){
        uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
        lightIndex = min(lightIndex, lightCount);
        sharedLights[gl_LocalInvocationIndex] = pointLight[lightIndex];
        barrier();

        for(int j=0;j<threadCount;j++){
            if(detectSphereAABB(j,clusterIndex)){
                visibleLightIndices[visibleLightCount]=i*threadCount+j;
                visibleLightCount+=1;
            }
        }
    }

    barrier();

    uint offset = atomicAdd(globalIndexCount, visibleLightCount);

    for(uint i = 0; i < visibleLightCount; ++i){
        lightIndexList.data[offset + i] = visibleLightIndices[i];
    }

    lightGrids.data[clusterIndex].offset = offset;
    lightGrids.data[clusterIndex].count = visibleLightCount;
}

bool detectSphereAABB(uint light, uint cluster){
    float radius = sharedLights[light].position_radius.w;
    vec4 pos = vec4(sharedLights[light].position_radius.xyz,1.0);
    vec3 center  = vec3(viewMatrix * pos);
    float squaredDistance = squaredDistancePointAndAABB(center, cluster);

    return squaredDistance <= (radius * radius);
}

float squaredDistancePointAndAABB(vec3 point, uint cluster){
    float squaredDis=0.0;
    ClusterAABBVolume curAABB=clusterAABBs.data[cluster];
    clusterAABBs.data[cluster].maxPoint[3]=cluster;
    for(int i=0;i<3;i++){
        float x=point[i];
        if(x<curAABB.minPoint[i]){
            squaredDis+=(curAABB.minPoint[i]-x)*(curAABB.minPoint[i]-x);
        }

        if(x>curAABB.maxPoint[i]){
            squaredDis+=(x-curAABB.maxPoint[i])*(x-curAABB.maxPoint[i]);
        }
    }

    return squaredDis;
}