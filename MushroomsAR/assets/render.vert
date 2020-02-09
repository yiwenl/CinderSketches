#version 300 es

uniform mat4    ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec2        uViewport;
uniform sampler2D   uColorMap;
uniform float       uParticleSize;

in vec4            ciPosition;
in vec3            iPositionOrg;
in vec3            iRandom;
in float        iLife;

out vec3        vColor;
out vec4        vShadowCoord;
out float       vLife;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                                0.0, 0.5, 0.0, 0.0,
                                0.0, 0.0, 0.5, 0.0,
                                0.5, 0.5, 0.5, 1.0 );


const float radius = 0.004;


void main( void )
{
	gl_Position    = ciModelViewProjection * ciPosition;
    
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale = mix(0.5, 1.0, iRandom.x);
    float lifeScale = smoothstep(0.5, 0.4, abs(iLife - 0.5));
    gl_PointSize = distOffset * lifeScale * scale * uParticleSize;
    
    vShadowCoord = ( biasMatrix * uShadowMatrix ) * ciPosition;

    vLife = iLife;
    
    vec2 uv = vec2(iPositionOrg.xy / vec2(0.2, 0.294)) + .5;
    uv.y = 1.0 - uv.y;
    vColor = texture(uColorMap, uv).rgb;
}
