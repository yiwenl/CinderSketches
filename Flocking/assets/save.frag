#version 330 core

in vec3 vPosition;
in vec3 vData;
in vec3 vExtra;

layout (location = 0) out vec4 oFragColor0;
layout (location = 1) out vec4 oFragColor1;
layout (location = 2) out vec4 oFragColor2;
layout (location = 3) out vec4 oFragColor3;

#define PI 3.141592653

void main( void )
{
    oFragColor0 = vec4(vPosition, 1.0);
    oFragColor1 = vec4(vExtra.zxy * 0.01, 1.0);
    oFragColor2 = vec4(vData, 1.0);

    vec3 extra = vExtra * .5 + .5;
    extra.x *= PI;
    oFragColor3 = vec4(extra, 1.0);
}
