#version 150 core

in vec2 vTextureCoord;
uniform float uTimestep;
uniform float uDissipation;
uniform vec2 uTexelSize;      // 1 / grid scale 

uniform sampler2D textureVel;  // input textureVel
uniform sampler2D textureMap;  // quantity to advect

out vec4 oColor;

void main() {
	vec2 pos = vTextureCoord - uTimestep * uTexelSize * texture(textureVel, vTextureCoord).xy;
	oColor = uDissipation * texture(textureMap, pos);
}