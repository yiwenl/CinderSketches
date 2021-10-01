#version 150 core
#include "./fragments/pointSize.glsl"

uniform mat4	ciModelViewProjection;

in vec2 inUV;

uniform sampler2D texturePos;
uniform sampler2D textureData;

out vec3 vColor;

const float radius = 0.04;

#define PI 3.141592653

void main( void )
{
    vec3 pos = texture(texturePos, inUV).xyz;
    vec3 data = texture(textureData, inUV).xyz;
	gl_Position	= ciModelViewProjection * vec4(pos, 1.0);


    vColor = vec3(mix(0.5,1.0, data.z));
    
    float distOffset = pointSize(gl_Position, radius);
    gl_PointSize = distOffset;
}
