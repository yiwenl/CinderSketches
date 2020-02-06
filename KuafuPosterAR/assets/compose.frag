#version 100

precision highp float;

uniform vec3 uColor;
uniform sampler2D uMap;
uniform sampler2D uMaskMap;

varying highp vec2 TexCoord0;

void main( void )
{    

	vec2 uv = TexCoord0;
	uv.y  = 1.0 - uv.y;
	vec4 color = texture2D(uMap, uv);
	// color.a = 1.0;
	color.a = smoothstep(0.0, 0.9, color.a);
	float mask = texture2D(uMaskMap, uv).r;
	// mask = step()

	// color.rgb *= mix(mask, 1.0, .5);
	color.a *= mask;

	gl_FragColor = color;
}
