#version 100

precision highp float;

uniform sampler2D 		uColorMap;
uniform sampler2D       uNormalMap;
uniform sampler2D       uAOMap;

varying highp vec2		TexCoord0;
varying highp vec3		Normal;

void main( void )
{
	vec3 normal     = normalize( -Normal );
	float diffuse   = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
    float ao        = texture2D(uAOMap, TexCoord0).r;
    
	vec3 color      = texture2D( uColorMap, TexCoord0 ).rgb * diffuse * ao;
    color *= 1.5;
    
	gl_FragColor = vec4( color, 1.0 );
}
