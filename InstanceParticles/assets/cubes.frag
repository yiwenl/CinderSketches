
in vec2	vTexCoord0;
in vec3 vNormal;

out vec4 fragColor;


float diffuse( vec3 N, vec3 L ) {
	float d = dot(normalize(N), normalize(L));
	return max(d, 0.0);
}

#define LIGHT vec3(0.2, 1.0, -0.3)

void main()
{
	float d = diffuse(vNormal, LIGHT);
	d = mix(d, 1.0, .75);
	fragColor = vec4(vec3(d), 1.0);
}
