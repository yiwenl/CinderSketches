#version 150 core

uniform mat4	ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;
out vec2 vUV;



void main( void )
{   
    vec2 pos = ciPosition.xz;
	// gl_Position	= ciModelViewProjection * ciPosition;
	gl_Position	= vec4(pos, 0.0, 1.0);

    vUV = ciTexCoord0;
}
