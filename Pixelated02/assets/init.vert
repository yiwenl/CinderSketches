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

vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, s, -s, c);
    return m * v;
}

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 rotate(vec3 v, vec3 axis, float angle) {
    mat4 m = rotationMatrix(axis, angle);
    return (m * vec4(v, 1.0)).xyz;
}


#define PI 3.141592653

void main()
{
    position        = iPosition;
    positionOrg     = iPositionOrg;
    velocity        = iVelocity;
    extra           = iExtra;


    vec3 pos = position;
    pos.yz = rotate(pos.yz, PI * 0.5);
    pos += uTranslate;

    vec4 screenPos = uShadowMatrix * uModelMatrix * vec4(pos, 1.0);
    vec2 screenUV = screenPos.xy / screenPos.w * .5 + .5;

    color           = texture(uShadowMap, screenUV).rgb;
}
