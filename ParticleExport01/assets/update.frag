#version 330 core
#include "./fragments/curlNoise.glsl"
#include "./fragments/map.glsl"

in vec2 vUV;

uniform sampler2D uTexPos;
uniform sampler2D uTexVel;
uniform sampler2D uTexData;
uniform sampler2D uTexExtra;
uniform float uTime;
uniform vec3 uCenter;

layout (location = 0) out vec4 oFragColor0;
layout (location = 1) out vec4 oFragColor1;
layout (location = 2) out vec4 oFragColor2;
layout (location = 3) out vec4 oFragColor3;


#define PI 3.141592653
#define PI2 PI * 2.0
#define MAX_RADIUS 2.5

void main( void )
{
    vec3 pos = texture(uTexPos, vUV).xyz;
    vec3 vel = texture(uTexVel, vUV).xyz;
    vec3 data = texture(uTexData, vUV).xyz;
    vec3 extra = texture(uTexExtra, vUV).xyz;

    
    float speedOffset = mix(1.5, 1.6, extra.x);
    vec3 acc = vec3(0.0);

    float posOffset = snoise(pos + uTime) * .5 + .5;
    posOffset = mix(1.0, 1.2, posOffset) * 0.2;
    vec3 noise = curlNoise(pos * posOffset - uTime);
    acc += noise;

    // pulling
    float d = distance(uCenter, pos);
    vec3 dir = normalize(uCenter - pos);
    float f = smoothstep(MAX_RADIUS * 0.5, MAX_RADIUS, d);
    acc += dir * f;

    
    vel += acc * speedOffset * 0.0004;

    pos += vel;
    vel *= 0.986;


    oFragColor0 = vec4(pos, 1.0);
    oFragColor1 = vec4(vel, 1.0);
    oFragColor2 = vec4(data, 1.0);
    oFragColor3 = vec4(extra, 1.0);
}
