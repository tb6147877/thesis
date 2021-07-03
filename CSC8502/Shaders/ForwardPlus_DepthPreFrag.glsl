#version 430 core

uniform bool isCluster;
uniform float zFar;
uniform float zNear;
uniform uint frameIndex;

layout (std430, binding = 2) buffer ClusterBasicData{
	mat4 inverseProj;
    uvec4 clusterSizes;
    uvec2 screenSizes;
    float scale;
    float bias;
};

layout (std430, binding = 7) buffer ActiveClusterListSSBO{
    uint data[];
} activeClusterList;

float linearDepth(float depth);

void main() {
	// We are not drawing anything to the screen, so nothing to be done here
	if(isCluster){

		uint slice = uint(max(log2(linearDepth(gl_FragCoord.z)) * scale + bias, 0.0));
    	uvec3 cluster = uvec3(uvec2( gl_FragCoord.xy / clusterSizes[3] ), slice);
    	uint clusterIndex = cluster.x + clusterSizes.x * cluster.y + (clusterSizes.x * clusterSizes.y) * cluster.z;

		activeClusterList.data[clusterIndex]=frameIndex;
	}
}

float linearDepth(float depth){
    float depthRange = 2.0 * depth - 1.0;
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}