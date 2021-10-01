#version 150 core

precision highp float;

in vec2 vTextureCoord;
uniform vec2 uTexelSize;
uniform sampler2D texturePressure;
uniform sampler2D textureVel;

out vec4 oColor;

void main() {
	float pL     = texture(texturePressure, vTextureCoord - vec2(uTexelSize.x, 0.0)).x;
	float pR     = texture(texturePressure, vTextureCoord + vec2(uTexelSize.x, 0.0)).x;
	float pB     = texture(texturePressure, vTextureCoord - vec2(0.0, uTexelSize.y)).x;
	float pT     = texture(texturePressure, vTextureCoord + vec2(0.0, uTexelSize.y)).x;
	vec2 v       = texture(textureVel, vTextureCoord).xy;

	oColor = vec4(v - vec2(pR - pL, pT - pB), 0.0, 1.0);
}
