#version 100

precision highp float;

uniform vec3 uColor;

varying highp vec2		TexCoord0;
varying highp vec3		Normal;

void main( void )
{
	vec3 normal     = normalize( -Normal );
	float diffuse   = max( dot( normal, vec3( 0.2, 1, .6 ) ), 0.0 );
    
    diffuse = mix(diffuse, 1.0, .5);
    
    vec3 color = uColor * diffuse;
    
	gl_FragColor = vec4( 1.0 );
}
