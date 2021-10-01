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

uniform sampler2D uShadowMap;
uniform mat4 uShadowMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uTranslate;

#include "./fragments/const.glsl"
#include "./fragments/rotate.glsl"


void main()
{
    positionOrg     = iPositionOrg + uTranslate;
    velocity        = vec3(0.0);
    extra           = iExtra;
    

    position = iPositionOrg + uTranslate;
    vec3 pos = position;

    vec4 screenPos = uShadowMatrix * uModelMatrix * vec4(pos, 1.0);
    vec2 screenUV = screenPos.xy / screenPos.w * .5 + .5;

    color           = texture(uShadowMap, screenUV).rgb;
}
