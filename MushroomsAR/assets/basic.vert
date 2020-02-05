#version 100

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

attribute vec4		ciPosition;
attribute vec2		ciTexCoord0;
attribute vec3		ciNormal;

uniform vec2 uSize;

varying lowp vec4	Color;
varying highp vec3	Normal;
varying highp vec2	TexCoord0;

void main( void )
{
  vec4 pos = ciPosition;
	gl_Position	= ciModelViewProjection * pos;
	TexCoord0 	= ciTexCoord0;
	Normal		= ciNormalMatrix * ciNormal;
}
