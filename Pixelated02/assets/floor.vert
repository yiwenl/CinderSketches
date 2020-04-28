#version 300 es

uniform mat4    ciModelViewProjection;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec3    uPosition;

in vec4            ciPosition;
out vec4           vShadowCoord;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0 );

const float radius = 0.0025;


void main( void )
{
  vec4 pos = ciPosition;
//    pos.xz *= 0.25;
  pos.xyz += uPosition;

  gl_Position         = ciModelViewProjection * pos;
    
  vShadowCoord        = ( biasMatrix * uShadowMatrix ) * ciPosition;
//    vShadowCoord        = ( uShadowMatrix * ciModelMatrix ) * ciPosition;
}
