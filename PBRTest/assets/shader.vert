#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;
uniform mat4  ciViewMatrix;
uniform mat4  ciProjectionMatrix;
uniform mat4  ciModelMatrix;
uniform mat4	ciViewMatrixInverse;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;

out lowp vec4	Color;
out highp vec3 vNormal;
out highp vec2 vTextureCoord;
out highp vec3 vPosition;
out highp vec3 vEyePosition;

void main( void )
{
	vec4 wsPos 	= ciModelMatrix * ciPosition;
	vec4 vsPos	= ciViewMatrix * wsPos;
	vPosition 	= wsPos.xyz / wsPos.w;

	gl_Position		= ciModelViewProjection * ciPosition;
	vTextureCoord	= ciTexCoord0;
	
	// vNormal       = normalize(vec3(ciModelMatrix * vec4(ciNormal, 0.0)));
	vNormal = ciNormalMatrix * ciNormal;


	vec4 eyeDirViewSpace	= vsPos - vec4( 0, 0, 0, 1 );
	vEyePosition			= -vec3( ciViewMatrixInverse * eyeDirViewSpace );
}
