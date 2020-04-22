#version 150 core


in vec3 inPosition;
in vec2 inUV;
in vec3 inExtra;
in vec3 inData;

out vec3 vPosition;
out vec3 vExtra;
out vec3 vData;

void main( void )
{
	gl_Position	= vec4(inUV, 0.0, 1.0);
	
    vPosition = inPosition;
    vData = inData;
    vExtra = inExtra;
}
