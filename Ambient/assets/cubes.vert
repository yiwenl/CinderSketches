
#version 150 core

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;
in vec3 aPosOffset;
in vec3 aExtra;

uniform mat3 ciNormalMatrix;
uniform mat4 ciModelViewProjection;
uniform float uRange;
uniform float uTime;

out vec4	vPosition;
out vec3	vNormal;
out vec2	vTexCoord0;
out vec3    vColor;

void main() {
    vec4 pos        = ciPosition;
    float scale     = mix(0.1, 1.0, aExtra.x) * 0.5;
    pos.xy          *= scale;
    float length    = mix(0.1, 10.0, aExtra.y) * 1.0;
    pos.z           *= length;


    vec3 posOffset  = aPosOffset;
    float speed     = mix(0.25, 1.0, aExtra.z) * 1.25;
    posOffset.z     += uTime * speed;
    posOffset.z     = mod(posOffset.z + uRange, uRange * 2.0) - uRange;
    pos.xyz         += posOffset;
    
    gl_Position		= ciModelViewProjection * pos;
	vNormal			= normalize( ciNormalMatrix * ciNormal );
    vTexCoord0 		= ciTexCoord0;

    vColor = vec3(mix(0.8, 1.0, aExtra.z));
}
