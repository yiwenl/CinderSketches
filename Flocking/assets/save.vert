#version 150 core


in vec3 inPosition;
in vec2 inUV;
in vec3 inExtra;

out vec3 vPosition;

void main( void )
{
	gl_Position	= vec4(inUV, 0.0, 1.0);
	
    vPosition = inPosition + inExtra * 0.001;
}
