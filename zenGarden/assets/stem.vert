// stem.vert
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

out vec3    Normal;
out vec2    TexCoord0;


#define PI 3.141592653
#define ZERO vec3(0.0)
#define DEFAULT_SCALE 0.01

#include "./fragments/rotate.glsl"
#include "./fragments/bezier.glsl"
#include "./fragments/curlNoise.glsl"

#define radius 0.025

void main( void )
{
  vec3 pos = vec3(radius, 0.0, 0.0);
  
  float a = ciTexCoord0.x * PI * 2.0;
  pos.xz = rotate(pos.xz, a);

  float offset = clamp(uOffset * 2.0 - 1.0, 0.0, 1.0);
    

  vec3 posOffset = bezier(vec3(0.0), uControl0, uControl1, uEnd, ciTexCoord0.y * offset);
  pos += posOffset;


  pos *= DEFAULT_SCALE;
  
  gl_Position     = ciModelViewProjection * vec4(pos, 1.0);
  // gl_Position     = ciModelViewProjection * ciPosition;
  TexCoord0       = ciTexCoord0;
  Normal          = ciNormalMatrix * ciNormal;
    
}
