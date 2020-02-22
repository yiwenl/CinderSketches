#version 300 es

precision highp float;

in highp vec3   iPosition;
in highp vec3   iPositionOrg;
in highp vec3   iColor;
in highp vec3   iExtra;

uniform mat4 uMatrix;
uniform mat4 uAlignMatrix;
uniform vec3 uCameraPos;
uniform vec3 uLookDir;
uniform float uHasBegin;
uniform sampler2D uEnvMap;

out vec3  position;
out vec3  positionOrg;
out vec3  color;
out vec3  extra;

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
#define FRONT vec3(0.0, 0.0, -1.0)

void main()
{
    vec3 pos    = iPosition;

    vec3 _extra = iExtra;
    vec3 _color = iColor;


    vec3 posToCamera = iPosition - uCameraPos;
    float d = dot(posToCamera.xz, uLookDir.xz);
    d = step(0.0, d);
    

    if(d <= 0.2) { 
        // need to reset position
        pos = uCameraPos + (uAlignMatrix * vec4(iPositionOrg, 1.0)).xyz;

        // set flag = reset colour
        _extra.x = 0.0;
    }


    if(_extra.x < 0.5) {
        // need to get new color;
        vec4 posScreen = uMatrix * vec4(pos, 1.0);
        vec2 uv = posScreen.xy / posScreen.w * .5 + .5;
        _color = texture(uEnvMap, uv).rgb;

        // set flag to avoid reset colour
        _extra.x = uHasBegin;
    }
    

    color = _color;
    extra = _extra;
    position = pos;
    positionOrg = iPositionOrg;
}
