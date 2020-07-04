
#version 400

layout(location = 0) in vec3 ciPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 ciModelViewProjection;
out vec3 vColor;

void main() {

    gl_Position		= ciModelViewProjection * vec4(ciPosition, 1.0);
    vColor          = aColor;
	gl_PointSize    = 5.0;
}
