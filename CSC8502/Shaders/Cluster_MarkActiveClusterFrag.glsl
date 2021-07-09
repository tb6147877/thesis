#version 430 core

layout (std430, binding = 2) buffer ClusterBasicData{
	mat4 inverseProj;
    uvec4 clusterSizes;
    uvec2 screenSizes;
    float scale;
    float bias;
};

layout (std430, binding = 7) buffer ActiveClustersSSBO{
    uint data[];
} activeClusters;

uniform sampler2D depthTex;
uniform vec2 pixelSize;
uniform float zFar;
uniform float zNear;

float linearDepth(float depth);

void main(){
    vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
 	float depth = texture(depthTex, texCoord.xy).r;

    uint slice = uint(max(log2(linearDepth(depth)) * scale + bias, 0.0));
    uvec3 cluster = uvec3(uvec2( gl_FragCoord.xy / clusterSizes[3] ), slice);
    uint clusterIndex = cluster.x + clusterSizes.x * cluster.y + (clusterSizes.x * clusterSizes.y) * cluster.z;

    activeClusters.data[clusterIndex]=1;
}

float linearDepth(float depth){
    float depthRange = 2.0 * depth - 1.0;
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}