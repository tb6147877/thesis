#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;//only one thread in the work group

struct ClusterAABBVolume{
	vec4 minPoint;
	vec4 maxPoint;
};

layout (std430, binding=1) writeonly buffer ClusterAABB{
	ClusterAABBVolume data[];
}clusterAABBs;

layout (std430, binding=2) readonly buffer ClusterBasicData{
	mat4 inverseProj;
    uvec4 clusterSizes;
    uvec2 screenSizes;
	float scale;
    float bias;
};

uniform float zNear;
uniform float zFar;

vec4 screen2View(vec4 screen);
vec4 clip2View(vec4 clip);
vec3 lineIntersection2ZPlane(vec3 A, vec3 B, float z);

//position of eye in view space
const vec3 eyePos=vec3(0.0);

void main(){
	//width and height size in pixle of cluster
	uint clusterSizeX=clusterSizes[3];

	//get cluster 1D index
	uint clusterIndex=gl_WorkGroupID.x +
					  gl_WorkGroupID.y*gl_NumWorkGroups.x+
					  gl_WorkGroupID.z*gl_NumWorkGroups.x*gl_NumWorkGroups.y;

	//get min and max point in screen space
	vec4 maxPointSs=vec4(vec2(gl_WorkGroupID.x+1,gl_WorkGroupID.y+1)*clusterSizeX,-1.0,1.0);//top right
	vec4 minPointSs=vec4(gl_WorkGroupID.xy*clusterSizeX,-1.0,1.0);//bottom left

	//transform to view space
	vec3 maxPointVs=screen2View(maxPointSs).xyz;
	vec3 minPointVs=screen2View(minPointSs).xyz;

	//get depth of near and far plane, based on the formula
	float nearDepth=-zNear*pow(zFar/zNear,gl_WorkGroupID.z/float(gl_NumWorkGroups.z));
	float farDepth=-zNear*pow(zFar/zNear,(gl_WorkGroupID.z+1)/float(gl_NumWorkGroups.z));

	//get 4 intersection points of max point&min point and near&far
	vec3 minPointNear=lineIntersection2ZPlane(eyePos, minPointVs, nearDepth);
	vec3 minPointFar=lineIntersection2ZPlane(eyePos, minPointVs, farDepth);
	vec3 maxPointNear=lineIntersection2ZPlane(eyePos, maxPointVs, nearDepth);
	vec3 maxPointFar=lineIntersection2ZPlane(eyePos, maxPointVs, farDepth);

	//get min point and amx point of the cluster of the AABB
	vec3 minAABB=min(min(minPointNear,minPointFar),min(maxPointNear,maxPointFar));
	vec3 maxAABB=max(max(minPointNear,minPointFar),max(maxPointNear,maxPointFar));

	//assign value
	clusterAABBs.data[clusterIndex].minPoint=vec4(minAABB,0.0);
	clusterAABBs.data[clusterIndex].maxPoint=vec4(maxAABB,0.0);
}

//from screen space to view space
vec4 screen2View(vec4 screen){
	// screen space to ndc
	vec2 texcoord=screen.xy/screenSizes;

	// ndc to clip space
	vec4 clip=vec4(texcoord*2.0-1.0,screen.z,screen.w);

	return clip2View(clip);
}

//from clip space to view space
vec4 clip2View(vec4 clip){
	//clip space to view space
	vec4 view=inverseProj*clip;

	//perspective division
	view=view/view.w;

	return view;
}

//get a intersection from a line and a z plane
vec3 lineIntersection2ZPlane(vec3 A, vec3 B, float z){
	//generate normal
	vec3 normal=vec3(0.0,0.0,1.0);

	//generate line
	vec3 ab=B-A;

	float len=(z-dot(normal,A))/dot(normal,ab);

	vec3 result=A+len*ab;

	return result;
}
