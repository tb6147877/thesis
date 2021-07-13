#version 430 core

layout (std430, binding = 2) readonly buffer ClusterBasicData{
	mat4 inverseProj;
    uvec4 clusterSizes;
    uvec2 screenSizes;
    float scale;
    float bias;
};

layout (std430, binding = 7) writeonly buffer ActiveClustersSSBO{
    uint data[];
} activeClusters;

in vec2 Texcoord;

uniform sampler2D depthTex;
uniform float zFar;
uniform float zNear;

float linearDepth(float depth);
out vec4 fragColour;

void main(){
 	float depth = texture(depthTex,Texcoord).r;

    uint slice = uint(max(log2(linearDepth(depth)) * scale + bias, 0.0));
    uvec3 cluster = uvec3(uvec2( gl_FragCoord.xy / clusterSizes[3] ), slice);
    uint clusterIndex = cluster.x + clusterSizes.x * cluster.y + (clusterSizes.x * clusterSizes.y) * cluster.z;

    activeClusters.data[clusterIndex]=1;

    fragColour=vec4(clusterIndex,0.0,0.0,1.0);
}

float linearDepth(float depth){
    float depthRange = 2.0 * depth - 1.0;
    float linear = 2.0 * zNear * zFar / (zFar + zNear - depthRange * (zFar - zNear));
    return linear;
}