#version 100

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

attribute vec4		ciPosition;
attribute vec2		ciTexCoord0;
attribute vec3		ciNormal;

uniform vec2 uSize;

uniform vec2 uScale;
uniform vec3 uTranslate;

varying lowp vec4	Color;
varying highp vec3	Normal;
varying highp vec2	TexCoord0;
varying highp vec2	vScreenCoord;

void main( void )
{
  vec4 pos 		= ciPosition;
	pos.xy 			*= uSize * uScale;
	pos.xyz 		+= uTranslate;
	gl_Position	= ciModelViewProjection * pos;
	vScreenCoord = gl_Position.xy / gl_Position.w * .5 + .5;
	TexCoord0 	= ciTexCoord0;
	Normal			= ciNormalMatrix * ciNormal;
}
