#version 430

struct VisibleIndex {
	int index;
};

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer{
	VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform int numberOfTilesX;
uniform int totalLightCount;

out vec4 fragColor;

#define MAX_NR_LIGHT 2048
#define NR_POINT_LIGHT 1000
#define TILE_SIZE 8
#define MAX_NR_LIGHT_PER_TILE 50

void main() {
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(TILE_SIZE, TILE_SIZE);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * MAX_NR_LIGHT_PER_TILE;
	uint i;
	for (i = 0; i < MAX_NR_LIGHT_PER_TILE && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);

	float ratio = float(i) /float(MAX_NR_LIGHT_PER_TILE);
	

	
	fragColor = vec4(vec3(ratio, ratio, ratio), 1.0);
}
