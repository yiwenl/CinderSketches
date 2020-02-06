#version 100

precision highp float;

uniform vec3 uColor;
uniform sampler2D uMap;
uniform float uOpacity;

varying highp vec2 TexCoord0;
varying highp vec2 vScreenCoord;

void main( void )
{    
	vec4 color = texture2D(uMap, TexCoord0);
	if(color.a>0.0) {
		color.rgb /= color.a;
	}

	color.a *= uOpacity;

	gl_FragColor = color;
}
