#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform mat4    uShadowMatrix;

uniform vec3    uPosition;

in vec4         ciPosition;
in vec2         ciTexCoord0;

out vec2        vUV;
out vec4        vShadowCoord;


const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0 );


void main( void )
{
  vec4 pos        = ciPosition;
  pos.xz          *= 3.0;
  pos.xyz         += uPosition;
  gl_Position     = ciModelViewProjection * pos;
  vUV             = ciTexCoord0;
    
  vShadowCoord        = ( biasMatrix * uShadowMatrix ) * pos;
}
