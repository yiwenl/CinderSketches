#version 150 core

uniform mat4	ciModelViewProjection;
uniform mat4    ciProjectionMatrix;

uniform vec2    uViewport;

in vec4			ciPosition;
in vec3			iRandom;

const float radius = 0.04;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
	
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale = mix(0.5, 1.0, iRandom.x);
    gl_PointSize = distOffset * scale;
}
