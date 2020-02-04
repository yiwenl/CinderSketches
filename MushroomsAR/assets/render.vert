#version 300 es

uniform mat4	ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;
uniform mat4    uTouchMatrix;

uniform vec2    uViewport;
uniform vec2    uSize;
uniform sampler2D uColorMap;

in vec4			ciPosition;
in vec3			iPositionOrg;
in vec3			iRandom;
in float        iLife;

out vec4        vShadowCoord;
out vec3        vColor;
out float       vLife;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                                0.0, 0.5, 0.0, 0.0,
                                0.0, 0.0, 0.5, 0.0,
                                0.5, 0.5, 0.5, 1.0 );

const float radius = 0.005;


void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
    vShadowCoord    = ( biasMatrix * uShadowMatrix * ciModelMatrix ) * ciPosition;

    
    vec2 uv = iPositionOrg.xy / uSize * .5 + .5;
    uv.y = 1.0 - uv.y;
    
    vColor = texture(uColorMap, uv).rgb;
    
    float colorScale = step(0.85, length(vColor));
    
    
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale = mix(0.5, 1.0, iRandom.x);
    float lifeScale = smoothstep(0.5, 0.4, abs(iLife - 0.5));
    gl_PointSize = distOffset * lifeScale * scale * colorScale;
    
    
    vLife = iLife;
}
