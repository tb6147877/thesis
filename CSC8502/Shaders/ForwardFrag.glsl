#version 330 core
out vec4 FragColor;

in Vertex {
	vec3 fragPos;
	vec2 texcoords;
	mat3 TBN;
	vec3 viewPos;
} IN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

struct PointLight{
    vec3 color;
    vec3 position;
    float radius;
};

#define NR_POINT_LIGHT 6
uniform PointLight pointLights[NR_POINT_LIGHT];

vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos);

void main(){
	vec3 result=vec3(0.0);
	vec3 viewDir=normalize(IN.viewPos-IN.fragPos);
	vec3 n=vec3(texture(texture_normal1,IN.texcoords))*2.0-1.0;
	vec3 normal=normalize(IN.TBN*normalize(n));
	for(int i=0;i<NR_POINT_LIGHT;i++){
		result+=calculatePointLight(pointLights[i],viewDir,normal,IN.fragPos);
	}

	FragColor=vec4(result,1.0);
}


vec3 calculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 fragPos){
	float dis=length(light.position-fragPos);
	float atten=1.0-clamp(dis/light.radius,0.0,1.0);

	vec3 result=vec3(0.0);
	vec3 lightDir=normalize(light.position-fragPos);
	float diff=max(dot(normal,lightDir),0.0);
	vec3 halfDir=normalize(viewDir+lightDir);
	float spec=pow(max(dot(halfDir,normal),0.0),32.0);

	vec3 ambient=light.color*vec3(texture(texture_diffuse1,IN.texcoords))*0.1;
	vec3 diffuse=light.color*vec3(texture(texture_diffuse1,IN.texcoords))*diff;
	vec3 specular=light.color*texture(texture_specular1,IN.texcoords).r*spec;

	ambient*=atten;
	diffuse*=atten;
	specular*=atten;

	result=ambient+diffuse+specular;

	return result;
}
