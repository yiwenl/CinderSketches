#version 410

in vec4 ciPosition;
in vec2 ciTexCoord0;

uniform mat4 ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    ciViewMatrix;

out vec2	vTexCoord0;

void main() {
	vec4 pos = ciPosition;
	float scale = 10.0;
	pos.xyz *= scale;

	mat4 matView = ciViewMatrix;
	matView[3][0] = 0.0;
	matView[3][1] = 0.0;
	matView[3][2] = 0.0;

	gl_Position		= ciProjectionMatrix * matView * ciModelMatrix * pos;
	vTexCoord0 		= ciTexCoord0;
}
