#version 300 es

precision highp float;

in vec3   iPosition;
in vec3   iVelocity;
in vec3   iPositionOrg;
in vec3   iRandom;
in vec3   iColor;
in float  iLife;

out vec3  position;
out vec3  velocity;
out vec3  positionOrg;
out vec3  random;
out vec3  color;
out float life;

uniform float uTime;
uniform float uSeed;
uniform float uOffset;
uniform vec3 uCenter;
uniform mat4 uShadowMatrix;

uniform sampler2D uEnvMap;

#include "./fragments/const.glsl"
#include "./fragments/rotate.glsl"
#include "./fragments/curlNoise.glsl"

void main()
{
    vec3 pos    = iPosition;
    vec3 vel    = iVelocity;

    float rotSpeed = 0.003;
    vec3 posOrg = iPositionOrg - uCenter;
    posOrg.xz = rotate(posOrg.xz, rotSpeed);
    posOrg.yz = rotate(posOrg.zy, -rotSpeed);
    posOrg += uCenter;
    

    float _life = iLife;
    _life -= mix(0.5, 1.0, iRandom.y) * 0.02;

    // forces
    vec3 acc = vec3(0.0);
    float posOffset = 1.0;

    vec3 adjustedPos = pos;
    float gap = 1.0;
    adjustedPos = floor(adjustedPos * gap) / gap;
    vec3 noise = curlNoise(adjustedPos * posOffset + uTime * 0.1 + uSeed);
    acc += noise * 0.2;


    // pulling back to center
    const float maxRadius = 1.5;
    float dist = length(pos - uCenter);
    float f = smoothstep(0.5, 2.0, dist);
    vec3 dir = normalize(pos - uCenter);
    acc -= dir * f;

    float speedOffset = mix(0.95, 1.0, iRandom.b);
    float initSpeedOffset = mix(1.0, 0.5, iLife);
    initSpeedOffset = mix(initSpeedOffset, 1.0, .1);
    vel += acc * 0.005 * initSpeedOffset * speedOffset;

    pos += vel;
    vel *= 0.97;

    vec3 _color = iColor;
    

    if(_life <= 0.0) {
        _life = 1.0;
        pos = posOrg;
        vel = vec3(0.0);

        vec4 screenPos = uShadowMatrix * vec4(pos, 1.0);
        vec2 uvScreen = screenPos.xy / screenPos.w * .5 + .5;

        _color       = texture(uEnvMap, uvScreen).rgb;
    }
    
    
    position    = pos;
    positionOrg = posOrg;
    velocity    = vel;
    random      = iRandom;
    color       = _color;
    life        = _life;
}
