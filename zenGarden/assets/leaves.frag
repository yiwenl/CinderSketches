#version 300 es

precision highp float;

in vec3    vColor;
in vec3    Normal;
in vec2    TexCoord0;


out highp vec4  oColor;


void main( void ) {
    oColor = vec4(TexCoord0, 0.0, 1.0);
}
