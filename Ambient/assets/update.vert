
#version 150 core

in vec3   iPosition;

out vec3  position;


uniform float uTime;

void main() {

    float r = length(iPosition.yz);
    float x = iPosition.x;

    float a = uTime + x * 0.2;
    float y = cos(a) * r;
    float z = sin(a) * r;

    position = vec3(x, y, z);

}
