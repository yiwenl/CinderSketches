#version 300 es

precision highp float;

in vec3         vColor;
out highp vec4  oColor;


void main( void ) {
    oColor = vec4(vColor, 1.0);
}
