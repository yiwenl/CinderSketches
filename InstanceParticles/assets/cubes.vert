
#version 150 core

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

// instancing
in vec3 aPosOffset0;
in vec3 aPosOffset1;
in vec3 aVel0;
in vec3 aVel1;
in vec3 aExtra;

uniform mat3 ciNormalMatrix;
uniform mat4 ciModelViewProjection;
uniform float uPivot;

out vec4	vPosition;
out vec3	vNormal;
out vec2	vTexCoord0;

#define HEAD vec3(1.0, 0.0, 0.0)
#define FRONT vec3(0.0, 0.0, 1.0)

mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 rotate(vec3 v, vec3 axis, float angle) {
	mat4 m = rotationMatrix(axis, angle);
	return (m * vec4(v, 1.0)).xyz;
}

void main() {
    vec4 pos        = ciPosition;
    float scaleYZ   = step(ciPosition.x, 0.0);
    scaleYZ = mix(scaleYZ, 1.0, .25);

    pos.xyz         *= 0.1;
    pos.yz          *= scaleYZ;

    vec3 vel        = mix(aVel0, aVel1, uPivot);
    float speed     = length(vel);
    float l         = smoothstep(0.0, 0.1, speed);
    l               = mix(l, 1.0, .1);
    pos.x           *= l * 4.0;
    l               = mix(0.5, 1.0, aExtra.y);
    pos.yz          *= l;

    vec3 dir = normalize(vel);

    // align
    vec3 axis = cross(HEAD, dir);
    float a = acos(dot(HEAD, dir));

    pos.xyz = rotate(pos.xyz, axis, -a);



    pos.xyz         += mix(aPosOffset0, aPosOffset1, uPivot);

    gl_Position		= ciModelViewProjection * pos;

    vec3 N          = rotate(ciNormal, axis, -a);
	//vNormal			= normalize( ciNormalMatrix * N );
	vNormal			= normalize( N );
    vTexCoord0 		= ciTexCoord0;

}
