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
uniform float uSeed;

#include "./fragments/const.glsl"
#include "./fragments/rotate.glsl"
#include "./fragments/curlNoise.glsl"

void main()
{
    vec3 pos        = iPosition;
    vec3 vel        = iVelocity;
    vec3 _extra     = iExtra;
    color           = iColor;

    positionOrg     = iPositionOrg;

    vec3 acc = vec3(0.0);

    // noise
     float posOffset = snoise(pos * 2.0 + vec3(0.0, 0.0, uSeed)) * .5 + .5;
     posOffset = mix(0.5, 2.5, posOffset) * 1.5;
//    float posOffset = 5.0;
    vec3 noise = curlNoise(pos * posOffset + uTime * 0.2);
    noise.y = (noise.y * .5 + .45) * 2.0;
    noise.xz *= 0.75;
    
    acc += noise * 0.5;
    
    // rotate force
    vec3 dir = normalize(pos * vec3(1.0, 0.0, 1.0));
    dir.xz = rotate(dir.xz, PI * 0.75);
    float d = length(pos.xz);
    float f = smoothstep(0.02, 0.1, d);
    acc += dir * f * 0.5;

    float speedOffset = mix(0.5, 1.0, _extra.g);
    float initSpeedOffset = smoothstep(0.0, 0.25, _extra.z);
    initSpeedOffset = mix(0.01, 1.0, initSpeedOffset);
    initSpeedOffset = pow(initSpeedOffset, 2.0);
//     vel += acc * speedOffset * 0.00025 * uOffset;
    vel += acc * speedOffset * 0.002 * uOffset * initSpeedOffset;

    vel *= 0.96;
    pos += vel;


    _extra.z += 0.01 * mix(0.5, 1.0, _extra.r);
    if(_extra.z > 1.0) {
//        _extra.z = 0.0;
//        pos = iPositionOrg;
//        vel = vec3(0.0);
    }


    position        = pos;
    velocity        = vel;
    extra           = _extra;
}
