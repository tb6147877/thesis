# version 330 core
in Vertex {
	vec4 colour;
} IN;

out vec4 fragColour;

void main ( void ) {
	//challenge-Interpolation 2
	//if(IN.colour.r<0.9&&IN.colour.r>0.1&&IN.colour.g<0.9&&IN.colour.g>0.1&&IN.colour.b<0.9&&IN.colour.b>0.1){
		//discard;
	//}else{
		//fragColour = IN.colour;
	//}

	//challenge-Interpolation 1
	//if(IN.colour.r<0.5&&IN.colour.g<0.5&&IN.colour.b<0.5){
		//discard;
	//}else{
		//fragColour = IN.colour;
	//}

	fragColour = IN.colour;
}