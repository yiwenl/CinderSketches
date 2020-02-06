#version 100

uniform mat4	ciModelViewProjection;

attribute vec4		ciPosition;
attribute vec2		ciTexCoord0;
attribute vec3		ciNormal;

varying highp vec3	Normal;
varying highp vec2	TexCoord0;

void main( void )
{
	vec4 pos = ciPosition;
	pos.xy *= 2.0;
	gl_Position	= pos;
	TexCoord0 	= ciTexCoord0;
	Normal			= ciNormal;
}
