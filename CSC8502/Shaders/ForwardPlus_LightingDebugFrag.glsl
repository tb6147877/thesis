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
#define NR_POINT_LIGHT 2000

void main() {
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * MAX_NR_LIGHT;
	uint i;
	for (i = 0; i < NR_POINT_LIGHT && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);

	float ratio = float(i) / float(totalLightCount/20);
	

	
	fragColor = vec4(vec3(ratio, ratio, ratio), 1.0);
}
