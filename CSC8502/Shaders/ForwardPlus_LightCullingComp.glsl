#version 430

struct PointLight {
	vec4 color;
	vec4 position_radius;
};

struct VisibleIndex {
	int index;
};

//ssbo
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) writeonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

#define MAX_NR_LIGHT 2048

//uniform
uniform sampler2D depthTex;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform ivec2 screenSize;
uniform int lightCount;

//shared values
shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;
shared vec4 frustumPlanes[6];
shared int visibleLightIndices[MAX_NR_LIGHT];
shared mat4 viewProjection;

#define TILE_SIZE 8
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;

void main(){
	//0.init
	ivec2 location = ivec2(gl_GlobalInvocationID.xy);
	ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
	ivec2 tileID = ivec2(gl_WorkGroupID.xy);
	ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
	uint index = tileID.y * tileNumber.x + tileID.x;

	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;//-1
		maxDepthInt = 0;
		visibleLightCount = 0;
		viewProjection = projMatrix * viewMatrix;
	}

	barrier();

	//1.calculate max and min depth of tile
	float maxDepth, minDepth;
	vec2 texcoord = vec2(location) / screenSize;
	float depth = texture(depthTex, texcoord).r;
	depth = (0.5 * projMatrix[3][2]) / (depth + 0.5 * projMatrix[2][2] - 0.5);

	uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);

	barrier();

	//2.use one thread calculate the frustum planes
	if (gl_LocalInvocationIndex == 0) {
		minDepth = uintBitsToFloat(minDepthInt);
		maxDepth = uintBitsToFloat(maxDepthInt);

		vec2 negativeStep = (2.0 * vec2(tileID)) / vec2(tileNumber);
		vec2 positiveStep = (2.0 * vec2(tileID + ivec2(1, 1))) / vec2(tileNumber);

		frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
		frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
		frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
		frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
		frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near
		frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far

		for (uint i = 0; i < 4; i++) {
			frustumPlanes[i] *= viewProjection;
			frustumPlanes[i] /= length(frustumPlanes[i].xyz);
		}

		frustumPlanes[4] *= viewMatrix;
		frustumPlanes[4] /= length(frustumPlanes[4].xyz);
		frustumPlanes[5] *= viewMatrix;
		frustumPlanes[5] /= length(frustumPlanes[5].xyz);
	}
	barrier();

	//3.Cull lights.
	uint threadCount = TILE_SIZE * TILE_SIZE;
	uint passCount = (lightCount + threadCount - 1) / threadCount;
	for (uint i = 0; i < passCount; i++) {
		uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
		if (lightIndex >= lightCount) {
			break;
		}

		vec4 position = vec4(lightBuffer.data[lightIndex].position_radius.xyz,1.0);
		float radius = lightBuffer.data[lightIndex].position_radius.w;

		float distance = 0.0;
		for (uint j = 0; j < 6; j++) {
			distance = dot(position, frustumPlanes[j]) + radius;//xyz of a plane is normal, w is distance

			if (distance <= 0.0) {
				break;
			}
		}

		if (distance > 0.0) {
			uint offset = atomicAdd(visibleLightCount, 1);
			visibleLightIndices[offset] = int(lightIndex);
		}
	}

	barrier();

	if (gl_LocalInvocationIndex == 0) {
		uint offset = index * MAX_NR_LIGHT; 
		for (uint i = 0; i < visibleLightCount; i++) {
			visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
		}

		if (visibleLightCount != MAX_NR_LIGHT) {
			visibleLightIndicesBuffer.data[offset + visibleLightCount].index = -1;
		}
	}
}