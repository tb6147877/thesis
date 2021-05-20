#version 330 core
layout (location = 0 ) in vec3 aPos;
layout (location = 1 ) in vec3 aNormal;
layout (location = 2 ) in vec2 aTexcoords;
layout (location = 3 ) in vec3 aTangent;
layout (location = 4 ) in vec3 aBitangent;


out Vertex {
	vec3 fragPos;
	vec2 texcoords;
	mat3 TBN;
	vec3 viewPos_tan;
	vec3 fragPos_tan;
} OUT ;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 viewPos;

void main(){
	OUT.fragPos=vec3(modelMatrix*vec4(aPos,1.0));
	OUT.texcoords=aTexcoords;
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);

	mat3 normalMat=mat3(transpose(inverse(modelMatrix)));
	vec3 tan=normalMat*aTangent;
	vec3 bitan=normalMat*aBitangent;
	vec3 norm=normalMat*aNormal;
	
	mat3 TBN=transpose(mat3(tan,bitan,norm));
	OUT.TBN=TBN;
	OUT.viewPos_tan=TBN*viewPos;
	OUT.fragPos_tan=TBN*OUT.fragPos;
    
}