#version 430

struct PointLight {
	vec4 color;
	vec4 position_radius;
};

//ssbo
layout(std430, binding = 0) readonly buffer LightSSBO {
	PointLight data[];
} lightList;

struct VisibleIndex {
	int index;
};

layout(std430, binding = 1) writeonly buffer VisibleLightIndicesSSBO {
	VisibleIndex data[];
} visibleLightIndicesList;

#define MAX_NR_LIGHT 2048
#define MAX_NR_LIGHT_PER_TILE 50

//uniform
uniform sampler2D depthTex;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
//uniform mat4 inverseProjView;
//uniform mat4 inverseView;

uniform ivec2 screenSize;
uniform int lightCount;

//shared values in workgroup
shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;
shared vec4 tileFrust[6];
shared int visibleLightIndices[MAX_NR_LIGHT_PER_TILE];
shared mat4 vpMat;

#define TILE_SIZE 8
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;

void main(){
	//0.init
	ivec2 location = ivec2(gl_GlobalInvocationID.xy);
	ivec2 tileID = ivec2(gl_WorkGroupID.xy);
	ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
	uint tileIndex = tileID.y * tileNumber.x + tileID.x;

	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;//-1
		maxDepthInt = 0;
		visibleLightCount = 0;
		vpMat = projMatrix * viewMatrix;
	}

	barrier();

	//1.calculate max and min depth of tile
	float maxDepth, minDepth;
	vec2 texcoord = vec2(location) / screenSize;
	float depth = texture(depthTex, texcoord).r;
	depth = (0.5 * projMatrix[3][2]) / (depth + 0.5 * projMatrix[2][2] - 0.5);

	uint depthInt = floatBitsToUint(depth);//convert to uint to do atomic function in work group
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);

	barrier();

	//2.use one thread calculate the frustum planes, this process same as CSC8502
	if (gl_LocalInvocationIndex == 0) {
		minDepth = uintBitsToFloat(minDepthInt);
		maxDepth = uintBitsToFloat(maxDepthInt);

		//map the plane's distance to -1,1. because need to calculate the ndc of the plane
		//in x and : 0,1 => *2 => 0,2 => -1 => -1,1; 0,1 => *2 => 0,2 => *-1 => -2,0 => +1 => -1,1
		vec2 negUnit = (2.0 * vec2(tileID)) / vec2(tileNumber);
		vec2 positiveUnit = (2.0 * vec2(tileID + ivec2(1, 1))) / vec2(tileNumber);

		//xyz of the plane is normal, and w is  distance
		tileFrust[0] = vec4(1.0, 0.0, 0.0, 1.0 - negUnit.x); // Left
		tileFrust[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveUnit.x); // Right
		tileFrust[2] = vec4(0.0, 1.0, 0.0, 1.0 - negUnit.y); // Bottom
		tileFrust[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveUnit.y); // Top
		tileFrust[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near, origin is 0,1, also map to -1,1 by *-1
		tileFrust[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far
		//now this is a cube in ndc, its range is -1,1 
		//*************************************

		for (uint i = 0; i < 4; i++) {
			//transform the cube from ndc into world space, it's left multiply and it means right multiply transpose matrix
			// its xyz is normal, so w(distance) is not important, so only 3x3 matrix of viewProjection is useful
			//so viewProjection's transpose matrix is inverse matrix, so it can transform ndc to world space
			tileFrust[i] *= vpMat;//it is tilted, so need view and projection matrix

			//these lines equal to above line
			//float temp=frustumPlanes[i].w;
			//frustumPlanes[i].w=1.0;
			//frustumPlanes[i] = inverseProjView*frustumPlanes[i];
			//frustumPlanes[i].xyz/=frustumPlanes[i].w;
			//frustumPlanes[i].w=temp;

			tileFrust[i] /= length(tileFrust[i].xyz);// normalize operation
		}

		tileFrust[4] *= viewMatrix;//near plane is not tilted, so it don't need projection matrix
		//float temp=frustumPlanes[4].w;
		//frustumPlanes[4].w=1.0;
		//frustumPlanes[4] = inverseView*frustumPlanes[4];
		//frustumPlanes[4].xyz/=frustumPlanes[4].w;
		//frustumPlanes[4].w=temp;
		tileFrust[4] /= length(tileFrust[4].xyz);// normalize operation

		tileFrust[5] *= viewMatrix;//far plane is not tilted, so it don't need projection matrix
		//temp=frustumPlanes[5].w;
		//frustumPlanes[5].w=1.0;
		//frustumPlanes[5] = inverseView*frustumPlanes[5];
		//frustumPlanes[5].xyz/=frustumPlanes[5].w;
		//frustumPlanes[5].w=temp;
		tileFrust[5] /= length(tileFrust[5].xyz);// normalize operation
	}
	barrier();

	//3.lights culling
	uint threadCount = TILE_SIZE * TILE_SIZE;
	uint passCount = (lightCount + threadCount - 1) / threadCount;
	for (uint i = 0; i < passCount; i++) {
		uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
		if (lightIndex >= lightCount) {
			break;
		}

		vec4 position = vec4(lightList.data[lightIndex].position_radius.xyz,1.0);
		float radius = lightList.data[lightIndex].position_radius.w;

		float distance = 0.0;
		for (uint j = 0; j < 6; j++) {
			//judge the intersection in world space
			distance = dot(position, tileFrust[j]) + radius;//xyz of a plane is normal, w is distance

			if (distance <= 0.0) {
				break;
			}
		}

		if (distance > 0.0 && visibleLightCount<=MAX_NR_LIGHT_PER_TILE) {
			uint offset = atomicAdd(visibleLightCount, 1);
			visibleLightIndices[offset] = int(lightIndex);
		}
	}

	barrier();

	if (gl_LocalInvocationIndex == 0) {
		uint offset = tileIndex * MAX_NR_LIGHT_PER_TILE; 
		for (uint i = 0; i < visibleLightCount; i++) {
			visibleLightIndicesList.data[offset + i].index = visibleLightIndices[i];
		}

		if (visibleLightCount != MAX_NR_LIGHT_PER_TILE) {
			visibleLightIndicesList.data[offset + visibleLightCount].index = -1;
		}
	}
}