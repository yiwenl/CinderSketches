#version 150 core

uniform mat4	ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec2    uViewport;

in vec4			ciPosition;

out vec3        color;
out vec4        vShadowCoord;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                                0.0, 0.5, 0.0, 0.0,
                                0.0, 0.0, 0.5, 0.0,
                                0.5, 0.5, 0.5, 1.0 );


const float radius = 0.05;

void main( void )
{
	gl_Position	= ciModelViewProjection * ciPosition;
	
    
    color = iRandom;
    
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    gl_PointSize = distOffset;
    
    vShadowCoord    = ( biasMatrix * uShadowMatrix * ciModelMatrix ) * ciPosition;
}
