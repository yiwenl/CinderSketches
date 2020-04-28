#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform mat3    ciNormalMatrix;
uniform float   uOffset;

uniform vec3 uEnd;
uniform vec3 uControl0;
uniform vec3 uControl1;

in vec4        ciPosition;
in vec2        ciTexCoord0;
in vec3        ciNormal;

// instancing
in vec3        aPosOffset;
in vec3        aEnd;
in vec3        aControl0;
in vec3        aControl1;
in vec3        aExtra;

out vec3    Normal;
out vec2    TexCoord0;
out vec3    vColor;


#define PI 3.141592653
#define ZERO vec3(0.0)
#define DEFAULT_SCALE 0.01

#include "./fragments/rotate.glsl"
#include "./fragments/bezier.glsl"
#include "./fragments/curlNoise.glsl"


void main( void )
{
    vec4 pos        = ciPosition;
    
    float offset = uOffset * 2.0 - aExtra.x;
    offset = clamp(offset, 0.0, 1.0);
    
    pos.y += 1.0;
    pos.xyz *= DEFAULT_SCALE;
    pos.x *= 0.3 * mix(0.8, 1.2, aExtra.y);
    pos.y *= 0.0;
    
    float t;
    
    if(ciTexCoord0.x < 0.5) {
        t = sin(ciTexCoord0.y * PI);
    } else {
        t = sin(ciTexCoord0.y * PI);
        float noise = snoise(vec3(ciTexCoord0.yy * 5.0 * offset, aExtra.x + aExtra.y)) * .5 + .5;
        t *= mix(0.1, 1.0, noise);
    }
    
    
    t = mix(t, 1.0, .1);
    pos.x *= t;
    
    float theta = mix(0.5, 1.5, aExtra.x) * 2.0 * PI;
    
    t = ciTexCoord0.y * theta * offset + aExtra.z * PI * 2.0;
    pos.xz = rotate(pos.xz, t);
    
    vec3 posOffset;
    
    
    
    posOffset = bezier(ZERO, aControl0, aControl1, aEnd, ciTexCoord0.y * offset) * DEFAULT_SCALE;
    
    
    pos.xyz += posOffset + aPosOffset * DEFAULT_SCALE;
    
    gl_Position     = ciModelViewProjection * pos;
    TexCoord0       = ciTexCoord0;
    Normal          = ciNormalMatrix * ciNormal;
    
    
    vColor = aPosOffset;
}
