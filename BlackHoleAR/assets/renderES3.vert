#version 300 es

uniform mat4	ciModelViewProjection;
uniform vec2    uViewport;

in vec4			ciPosition;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
    gl_PointSize = 14.0;
}
