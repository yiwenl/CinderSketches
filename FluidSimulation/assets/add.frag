#version 150 core

precision highp float;
in vec2 vTextureCoord;
uniform sampler2D textureBase;
uniform sampler2D textureAdd;
uniform float uStrength;

out vec4 oColor;

void main(void) {
    vec3 base = texture(textureBase, vTextureCoord).xyz;
    vec3 add = texture(textureAdd, vTextureCoord).xyz;
    oColor	 = vec4(base + add * uStrength, 1.0);
}