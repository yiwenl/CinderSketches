// stem.frag
#version 300 es

precision highp float;

in vec3    vExtra;
in vec3    Normal;
in vec2    TexCoord0;


out highp vec4  oColor;

#define GREEN vec3(0.5, 0.9, 0.5) * 0.4


void main( void ) {
  // oColor = vec4(TexCoord0, 0.0, 1.0);
  oColor = vec4(GREEN, 1.0);
}
