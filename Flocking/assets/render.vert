#version 150 core

uniform mat4	ciModelViewProjection;
uniform mat4    ciProjectionMatrix;

uniform vec2    uViewport;

in vec2 inUV;

uniform sampler2D texturePos;

const float radius = 0.04;

void main( void )
{
    vec3 pos = texture(texturePos, inUV).xyz;
	gl_Position	= ciModelViewProjection * vec4(pos, 1.0);
	
    float distOffset = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    gl_PointSize = distOffset;
}
