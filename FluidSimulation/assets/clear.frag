#version 150 core

precision highp float;

in vec2 vTextureCoord;
uniform sampler2D texturePressure;
uniform float uDissipation;

out vec4 oColor;

void main() {
    oColor = uDissipation * texture(texturePressure, vTextureCoord);
}