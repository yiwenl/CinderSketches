#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform vec3    uPosition;
uniform float   uScale;

in vec4         ciPosition;
in vec2         ciTexCoord0;
out vec2        vUV;

#define scale 0.5

void main( void ) {
  vec4 pos        = ciPosition;
  pos.xyz         *= scale * uScale;
  pos.xyz         += uPosition;
  gl_Position     = ciModelViewProjection * pos;
  vUV             = ciTexCoord0;
}
