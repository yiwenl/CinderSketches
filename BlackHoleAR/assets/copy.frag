#version 410

in vec2	vTexCoord0;

uniform sampler2D uColorMap;

out vec4 fragColor;

void main()
{
	fragColor = texture(uColorMap, vTexCoord0);
}
