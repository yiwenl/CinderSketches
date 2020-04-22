#version 300 es

precision highp float;

in highp vec3   iPosition;
in highp vec3   iPositionOrg;
in highp vec3   iVelocity;
in highp vec3   iColor;
in highp vec3   iExtra;

out vec3  position;
out vec3  positionOrg;
out vec3  velocity;
out vec3  color;
out vec3  extra;

uniform float uOffset;
uniform float uTime;

#include "./fragments/const.glsl"
#include "./fragments/rotate.glsl"
#include "./fragments/curlNoise.glsl"

void main()
{
    // vec2 tmp = -vec2(1.0, 0.0) * 0.01 * uOffset;
    vec3 pos        = iPosition;
    vec3 vel        = iVelocity;
    vec3 _extra     = iExtra;
    color           = iColor;

    positionOrg     = iPositionOrg;

    vec3 acc = vec3(0.0);

    // noise
    float posOffset = snoise(pos * 5.0 - uTime) * .5 + .5;
    posOffset = mix(2.5, 7.5, posOffset);
    // float posOffset = 5.0;
    vec3 noise = curlNoise(pos * posOffset + uTime * 0.2);
    noise.y = (noise.y * .5 + .5) * 2.0;

    acc += noise * 0.5;

    float speedOffset = mix(0.75, 1.0, _extra.g);
    float initSpeedOffset = smoothstep(0.0, 0.75, _extra.z);
    initSpeedOffset = mix(0.01, 1.0, initSpeedOffset);
    initSpeedOffset = pow(initSpeedOffset, 2.0);
    // vel += acc * speedOffset * 0.0005 * uOffset;
    vel += acc * speedOffset * 0.00075 * uOffset * initSpeedOffset;

    vel *= 0.96;
    pos += vel;


    _extra.z += 0.01;
    if(_extra.z > 1.0) {
        // _extra.z = 0.0;
        // pos = iPositionOrg;
        // vel = vec3(0.0);
    }


    position        = pos;
    velocity        = vel;
    extra           = _extra;
}
