#version 150 core

in vec4 ciPosition;
in vec3 aExtra;
//in vec3 ciNormal;
//in vec2 ciTexCoord0;

uniform mat3 ciNormalMatrix;
uniform mat4 ciModelViewProjection;

out vec4	vPosition;
//out vec3	vNormal;
//out vec2	vTexCoord0;

out vec3 vColor;

void main() {

    gl_Position		= ciModelViewProjection * ciPosition;
	//vNormal			= normalize( ciNormalMatrix * ciNormal );
    //vTexCoord0 		= ciTexCoord0;

    vColor = aExtra;
    gl_PointSize    = 5.0;
}
