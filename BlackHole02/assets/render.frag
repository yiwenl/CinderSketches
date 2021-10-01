#version 300 es

precision highp float;

#define uMapSize vec2(1024.0)

in vec4         vShadowCoord;
in vec3         vColor;



out highp vec4  oColor;

void main( void ) {
    
    oColor = vec4(vColor, 1.0);
}
