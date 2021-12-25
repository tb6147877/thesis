#version 330 core

in vec2 Texcoord;

uniform sampler2D diffTex0;
// uniform sampler2D diffTex1;
// uniform sampler2D diffTex2;

out vec4 fragColor;

void main() {
	//fragColor = vec4(1.0,0.0,0.0,1.0);
	//fragColor = vec4(texture(diffTex,Texcoord).rgb,1.0);
    fragColor = texture(diffTex0,Texcoord);

    // vec4 color0 = texture(diffTex0,Texcoord);
    // vec4 color1 = texture(diffTex1,Texcoord);
    // vec4 color2 = texture(diffTex2,Texcoord);
    
    // fragColor =color0+color1+color2;

}