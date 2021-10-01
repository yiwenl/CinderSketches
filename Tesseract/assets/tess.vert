#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform mat4    ciModelMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciProjectionMatrix;

uniform mat4    uMtxScale;
uniform float   uOffset;
uniform vec3    uPos;


in vec4         ciPosition;
in vec3         ciNormal;
in vec3         aPosOffset;
in vec3         aExtra;
in vec2         ciTexCoord0;

out vec2        vUV;
out vec3        vNormal;
out vec3        vExtra;
out vec3        vPosition;
out vec4        vScreenPos;
out vec4        vScreenPos2;
out vec4        vScreenPos3;


#define uGap 6.0


void main( void )
{
  float uFaceScale = mix(2.99, 1.0, uOffset);
  vec4 pos        = ciPosition;
  pos.xy          = clamp(pos.xy, vec2(-1.0), vec2(1.0));
  pos.xy          *= uFaceScale;

  float s = 0.001;
  vec4 pos2 = vec4(pos);
  pos2.x *= s;
  vec4 pos3 = vec4(pos);
  pos3.y *= s;

  pos             = ciModelMatrix * pos;
  pos2            = ciModelMatrix * pos2;
  pos3            = ciModelMatrix * pos3;

  pos.xyz         += uPos;
  pos2.xyz        += uPos;
  pos3.xyz        += uPos;
  vScreenPos      = ciProjectionMatrix * ciViewMatrix * uMtxScale * pos;
  vScreenPos2     = ciProjectionMatrix * ciViewMatrix * uMtxScale * pos2;
  vScreenPos3     = ciProjectionMatrix * ciViewMatrix * uMtxScale * pos3;

  pos             = ciPosition;
  pos.xy          *= uFaceScale;
  pos             = ciModelMatrix * pos;
  pos.xyz         += aPosOffset * uGap + uPos;

  gl_Position     = ciProjectionMatrix * ciViewMatrix * uMtxScale * pos;

  mat4 mtxModel = ciModelMatrix;
  mtxModel[3][0] = 0.0;
  mtxModel[3][1] = 0.0;
  mtxModel[3][2] = 0.0;

  vec4 n = mtxModel * vec4(ciNormal, 1.0);
  vNormal         = n.xyz;
  vExtra          = aExtra;
  vUV             = ciTexCoord0;

  vPosition = ciPosition.xyz;
}
