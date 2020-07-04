
#version 150 core

in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2	vTexCoord0;

void main() {

    gl_Position		= ciPosition;
    vTexCoord0 		= ciTexCoord0;

}
