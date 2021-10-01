#version 300 es

precision highp float;

#define uMapSize vec2(1024.0)

in vec2         vUV;

uniform sampler2D uColorMap;
uniform sampler2D uNormalMap;

out highp vec4  oColor;

#define COLOR_MOON vec3(1.0, 1.0, 0.98)


void main( void ) {
    vec4 color = texture(uColorMap, vUV);
	oColor = color;
}
