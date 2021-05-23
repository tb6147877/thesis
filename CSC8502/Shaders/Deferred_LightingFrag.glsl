#version 330 core

struct PointLight{
	vec3 color;
    vec3 position;
    float radius;
};

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform vec2 pixelSize;
uniform vec3 viewPos;

uniform PointLight pointLight;

uniform mat4 inverseProjView;

layout (location = 0) out vec4 diffuseOutput;
layout (location = 1) out vec4 specularOutput;

void main(){
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
 	float depth = texture(depthTex, texCoord.xy).r;
 	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
 	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
 	vec3 worldPos = invClipPos.xyz / invClipPos.w;

 	vec3 normal = normalize(texture(normTex, texCoord.xy).xyz * 2.0 - 1.0);
	vec3 viewDir = normalize(viewPos - worldPos);

	float dis=length(pointLight.position - worldPos);
	float atten=1.0-clamp(dis/pointLight.radius, 0.0, 1.0);

	vec3 lightDir = normalize(pointLight.position - worldPos);
	vec3 halfDir = normalize(viewDir+lightDir);

	float diff=max(dot(normal,lightDir),0.0);
	float spec=pow(max(dot(halfDir,normal),0.0),32.0);

	diffuseOutput=vec4(pointLight.color*diff*atten, 1.0);
	specularOutput=vec4(pointLight.color*spec*atten, 1.0);
}