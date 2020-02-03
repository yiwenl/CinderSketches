#version 300 es

uniform mat4	  ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;
uniform mat4    uTranslateMatrix;

in vec4			    ciPosition;

out vec4        vShadowCoord;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                                0.0, 0.5, 0.0, 0.0,
                                0.0, 0.0, 0.5, 0.0,
                                0.5, 0.5, 0.5, 1.0 );

void main( void )
{
  vec4 pos = vec4(ciPosition);
  pos.xyz = pos.yxz;
  pos.x += 2.5;

	gl_Position	= ciProjectionMatrix * ciViewMatrix * ciModelMatrix * pos;

  vShadowCoord    = ( biasMatrix * uShadowMatrix * ciModelMatrix ) * pos;
}
