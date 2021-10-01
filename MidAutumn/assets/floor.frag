#version 300 es

precision highp float;

#define uMapSize vec2(1024.0)

in vec2         vUV;

out highp vec4  oColor;

#define COLOR_MOON vec3(1.0, 1.0, 0.95)


void main( void ) {
    float d = distance(vUV, vec2(.5));
    d = smoothstep(0.5, 0.0, d);

    oColor = vec4(COLOR_MOON, 0.4 * d);
}
