#version 150 core

in vec3 vPosition;
out highp vec4 oColor;

void main( void )
{
    oColor = vec4(vPosition, 1.0);
}
