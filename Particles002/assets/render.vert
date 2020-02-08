#version 150 core

uniform mat4	ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec2    uViewport;
uniform sampler2D uColorMap;

in vec4			ciPosition;
in vec3			iPositionOrg;
in vec3			iRandom;
in float        iLife;

out vec3        vColor;
out vec4        vShadowCoord;
out float       vLife;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                                0.0, 0.5, 0.0, 0.0,
                                0.0, 0.0, 0.5, 0.0,
                                0.5, 0.5, 0.5, 1.0 );


const float radius = 0.002;

void main( void )
{
	// gl_Position	= ciProjectionMatrix * ciViewMatrix * ciPosition;
	gl_Position	= ciModelViewProjection * ciPosition;
	
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale = mix(0.5, 1.0, iRandom.x);
    float lifeScale = smoothstep(0.5, 0.4, abs(iLife - 0.5));
    gl_PointSize = distOffset * lifeScale * scale;
    
    vShadowCoord = ( biasMatrix * uShadowMatrix ) * ciPosition;

    vLife = iLife;
    
    vec2 uv = vec2(iPositionOrg.xy / 0.1) * .5 + .5;
    vColor = texture(uColorMap, uv).rgb;
}
