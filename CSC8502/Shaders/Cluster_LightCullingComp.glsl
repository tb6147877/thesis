#version 430 core

//layout(local_size_x = 18, local_size_y = 12, local_size_z = 6) in;
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

layout (std430, binding = 1) buffer ClusterAABB{
	ClusterAABBVolume data[];
} clusterAABBs;

layout(std430, binding = 3) buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout (std430, binding = 4) buffer LightIndexSSBO{
    uint data[];
} lightIndexList;

layout (std430, binding = 5) buffer LightGridSSBO{
    LightGrid data[];
} lightGrids;

layout (std430, binding = 6) buffer GlobalIndexCountSSBO{
    uint globalIndexCount;
};

layout (std430, binding = 7) buffer ActiveClusterListSSBO{
    uint data[];
} activeClusterList;

shared PointLight sharedLights[16*9*4];

uniform mat4 viewMatrix;
uniform uint frameIndex;

#define MAX_LIGHTS_PER_CLUSTER 50

bool detectSphereAABB(uint light, uint cluster);
float squaredDistancePointAndAABB(vec3 point, uint cluster);

void main(){
    globalIndexCount=0;
    uint lightCount  = lightBuffer.data.length();

    uint clusterIndex = gl_LocalInvocationIndex + gl_WorkGroupSize.x * gl_WorkGroupSize.y * gl_WorkGroupSize.z * gl_WorkGroupID.z;
    
    if(activeClusterList.data[clusterIndex]!=frameIndex){return;}//fliter through active cluster list

    uint visibleLightCount = 0;
    uint visibleLightIndices[MAX_LIGHTS_PER_CLUSTER];

    for(int i=0;i<lightCount;i++){
        if(detectSphereAABB(i,clusterIndex)){
                if(visibleLightCount>=MAX_LIGHTS_PER_CLUSTER){break;}
                visibleLightIndices[visibleLightCount]=i;
                visibleLightCount+=1;
            }
    }

    
    barrier();

    uint offset = atomicAdd(globalIndexCount, visibleLightCount);
    //uint offset = clusterIndex*50;

    for(uint i = 0; i < visibleLightCount; ++i){
        lightIndexList.data[offset + i] = visibleLightIndices[i];
    }

    lightGrids.data[clusterIndex].offset = offset;
    lightGrids.data[clusterIndex].count = visibleLightCount;
    
}

bool detectSphereAABB(uint light, uint cluster){
    float radius = lightBuffer.data[light].position_radius.w;
    vec4 pos = vec4(lightBuffer.data[light].position_radius.xyz,1.0);
    vec3 center  = vec3(viewMatrix * pos);
    float squaredDistance = squaredDistancePointAndAABB(center, cluster);

    return squaredDistance - (radius * radius) <= 0.0;
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