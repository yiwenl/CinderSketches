#version 300 es

precision highp float;

in vec3         vColor;
out highp vec4  oColor;

uniform float uOffset;

void main( void ) {
    if(uOffset <= 0.0) {
        discard;
    }
    oColor = vec4(vColor, 1.0);
}
