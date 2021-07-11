#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 7) buffer ActiveClustersSSBO{
    uint data[];
} activeClusters;

layout (std430, binding = 8) buffer DenseActiveClustersSSBO{
    uint data[];
} denseActiveClusters;

layout (std430, binding = 9) buffer ActiveClustersCountSSBO{
    uint activeClustersCount;
};

void main(){
    uint clusterIndex=gl_WorkGroupID.x +
					  gl_WorkGroupID.y*gl_NumWorkGroups.x+
					  gl_WorkGroupID.z*gl_NumWorkGroups.x*gl_NumWorkGroups.y;
    if(activeClusters.data[clusterIndex]==1){
        uint offset = atomicAdd(activeClustersCount, 1);
        denseActiveClusters.data[offset] = clusterIndex;
    }
    activeClusters.data[clusterIndex]=0;
}