#version 150 core

uniform mat4	ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec2    uViewport;

in vec4			ciPosition;
in vec3			iPositionOrg;
in vec3			iRandom;
in float        iLife;

out vec3        color;
out vec4        vShadowCoord;
out vec4        vScreenCoord;
out float       vLife;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                                0.0, 0.5, 0.0, 0.0,
                                0.0, 0.0, 0.5, 0.0,
                                0.5, 0.5, 0.5, 1.0 );


const float radius = 0.02;

void main( void )
{
    vec4 pos = ciPosition;
    pos.z -= 1.0;
	gl_Position	= ciModelViewProjection * pos;
	
    color = vec3(1.0);
    
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale = mix(0.5, 1.0, iRandom.x);
    float lifeScale = smoothstep(0.5, 0.4, abs(iLife - 0.5));
    gl_PointSize = distOffset * lifeScale * scale;
    
    vShadowCoord    = ( biasMatrix * uShadowMatrix * ciModelMatrix ) * pos;
    vScreenCoord    = ciModelViewProjection * vec4(iPositionOrg, 1.0);

    vLife = iLife;
}
