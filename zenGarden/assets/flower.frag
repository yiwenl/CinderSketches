#version 300 es

precision highp float;

in vec3    vExtra;
in vec3    Normal;
in vec2    TexCoord0;


out highp vec4  oColor;

#define RED vec3(0.9, vec2(0.5)) * 1.5


void main( void ) {
  // float g = mix(0.25, 0.5, vExtra.x);
  // oColor = vec4(vec3(g), 0.75);

  float a = mix(0.95, 0.75, vExtra.y);
  oColor = vec4(RED * mix(0.8, 1.0, vExtra.z), a);
}
