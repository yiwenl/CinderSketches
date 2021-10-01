#version 150 core

precision highp float;

in vec2 vTextureCoord; // grid coordinates    
uniform vec2 uTexelSize;
uniform sampler2D texturePressure;
uniform sampler2D textureDivergence;

out vec4 oColor;

void main() {
  // left, right, bottom, and top texturePressure samples
  float L = texture(texturePressure, vTextureCoord - vec2(uTexelSize.x, 0.0)).x;
  float R = texture(texturePressure, vTextureCoord + vec2(uTexelSize.x, 0.0)).x;
  float B = texture(texturePressure, vTextureCoord - vec2(0.0, uTexelSize.y)).x;
  float T = texture(texturePressure, vTextureCoord + vec2(0.0, uTexelSize.y)).x;

  // textureDivergence sample, from center
  float bC = texture(textureDivergence, vTextureCoord).x;
  
  // evaluate Jacobi iteration
  oColor = vec4(0.25 * (L + R + B + T - bC), 0, 0, 1);
}
