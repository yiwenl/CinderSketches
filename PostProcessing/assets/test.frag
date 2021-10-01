#version 150 core

precision highp float;

in vec3    Normal;
in vec2    TexCoord0;

uniform sampler2D uMap;
uniform float uOffset;

out vec4     oColor;

void main( void ) {
    vec2 uv = TexCoord0;
    uv.y = 1.0 - uv.y;
    uv.x += uOffset;
    oColor = texture(uMap, uv);
    // oColor = vec4(TexCoord0, 0.0, 1.0);
    // oColor = vec4(TexCoord0.xxx, 1.0);
}
