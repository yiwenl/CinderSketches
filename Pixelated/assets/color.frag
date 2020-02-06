#version 100

precision highp float;

varying vec3 Normal;


uniform vec3 uColor;
uniform float uOpacity;

#define LIGHT vec3(0.2, 1.0, 0.6)

void main( void )
{    
	float d = max(dot(normalize(Normal), normalize(LIGHT)), 0.0);
	d = mix(d, 1.0, .25);
	gl_FragColor = vec4( uColor * d, uOpacity);
}
