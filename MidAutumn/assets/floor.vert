#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;

in vec4         ciPosition;
in vec2         ciTexCoord0;
out vec2        vUV;

#define scale 0.3

void main( void ) {
  gl_Position     = ciModelViewProjection * ciPosition;
  vUV             = ciTexCoord0;
}
