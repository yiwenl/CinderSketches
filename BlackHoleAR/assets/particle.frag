#version 300 es
precision highp float;


in vec4 vColor;
in vec4 vPosition;
in vec3 vNormal;
in vec4 vModelPosition;
in vec3 vModelNormal;

uniform vec3 uLightPos;

out vec4 fragColor;

float diffuse(vec3 N, vec3 L ) {
    return max(
        dot(normalize(N), normalize(L))
    , 0.0);
}

float diffuse(vec3 N, vec3 L, float p ) {
    return mix(diffuse(N, L), 1.0, p);
}
void main()
{
    float d = diffuse(vNormal, uLightPos, .5);
	fragColor = vec4(vec3(d), 1.0);
}
