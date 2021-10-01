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


uniform mat4 uShadowMatrix;

uniform float uTime;
uniform vec3 uPosition;

uniform sampler2D uEnvMap;

#include "./fragments/const.glsl"
#include "./fragments/curlNoise.glsl"

void main()
{
    const float radius = 1.5;
    float gap = 4.0;
    vec3 adjustedPos = floor(iPosition/gap) * gap;
    vec3 pos = curlNoise(adjustedPos + iRandom * 0.05);
    pos *= radius + mix(-0.1, 0.1, iRandom.z);

    vec4 screenPos = uShadowMatrix * vec4(pos + uPosition, 1.0);
    vec2 uvScreen = screenPos.xy / screenPos.w * .5 + .5;

    // pos += iRandom * 0.5;
    life = 1.0;

    position    = pos + uPosition;
    positionOrg = pos + uPosition;
    velocity    = vec3(0.0);
    random      = iRandom;
    life        = iLife;
    color       = texture(uEnvMap, uvScreen).rgb;
}
