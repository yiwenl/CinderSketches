// flower.vert
#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform mat4    ciModelViewInverse;
uniform mat3    ciNormalMatrix;

uniform float   uOffset;
uniform float   uNum;
uniform float   uTime;
uniform vec3    uPos;

in vec4        ciPosition;
in vec2        ciTexCoord0;
in vec3        ciNormal;
in vec3        aPosOffset;

out vec3    vExtra;
out vec3    Normal;
out vec2    TexCoord0;


#define PI 3.141592653
#define ZERO vec3(0.0)
#define DEFAULT_SCALE 0.01

#include "./fragments/rotate.glsl"
#include "./fragments/bezier.glsl"
#include "./fragments/curlNoise.glsl"


void main( void )
{
  vec3 pos = ciPosition.xyz;
  pos.y += 1.0;
  pos.y *= 3.0;
  pos *= 0.3;
  

  float offset = clamp(uOffset * 2.0 - aPosOffset.y, 0.0, 1.0);
  pos *= offset;

  float s = sin(ciTexCoord0.y * PI) * mix(0.8, 1.0, aPosOffset.y);
    
    
  float noise = snoise(vec3(aPosOffset.x, uTime, aPosOffset.z * 0.1));
  

  pos.x *= s;
  float a = aPosOffset.x / (uNum - 1.0);

  float r = 0.2;
  float totalAngle = 3.0;
  a = -totalAngle * 0.5 + a * totalAngle + mix(-r, r, aPosOffset.z);
  a += noise * 0.1;
  pos.xy = rotate(pos.xy, a);
  pos.z = aPosOffset.x * 0.01;
    
    pos.xy = rotate(pos.xy, -PI * 0.5);
  pos = vec3(ciModelViewInverse * vec4(pos, 1.0));
  

  

  pos += uPos;
  pos.y -= 0.1;


  pos *= DEFAULT_SCALE;
  
  
  gl_Position     = ciModelViewProjection * vec4(pos, 1.0);
  TexCoord0       = ciTexCoord0;
  Normal          = ciNormalMatrix * ciNormal;
  vExtra          = vec3(aPosOffset.x / (uNum - 1.0), aPosOffset.yz);   
}
