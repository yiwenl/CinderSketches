
in vec2	vTexCoord0;
in vec3 vColor;
in vec3 vNormal;
out vec4 fragColor;

float diffuse(vec3 N, vec3 L) {
	float d = dot(normalize(N), normalize(L));
	d = max(d, 0.0);
	return d;
}

vec3 LIGHT = vec3(0.8, 1.0, -0.2);

void main()
{
	float d = diffuse(vNormal, LIGHT);

	d = mix(d, 1.0, .95);

	fragColor = vec4(vec3(d), 1.0);
}
