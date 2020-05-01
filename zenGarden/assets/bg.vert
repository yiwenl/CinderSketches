#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform mat3    ciNormalMatrix;
uniform float   uOffset;

in vec4        ciPosition;
in vec2        ciTexCoord0;
in vec3        ciNormal;

out vec3    Normal;
out vec2    TexCoord0;

void main( void )
{
    vec4 pos        = ciPosition;
    gl_Position     = pos;
    TexCoord0       = ciTexCoord0;
    Normal          = ciNormalMatrix * ciNormal;
}
