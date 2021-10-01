
in vec2	vTexCoord0;
in vec3 vNormal;
in vec3 vColor;

out vec4 fragColor;


float diffuse( vec3 N, vec3 L ) {
	float d = dot(normalize(N), normalize(L));
	return max(d, 0.0);
}

#define LIGHT vec3(0.2, 0.5, -1.5)
float exposure = 1.5;

void main()
{
	float d = diffuse(vNormal, -LIGHT);
	d = mix(d, 1.0, .5);
	//fragColor = vec4(vec3(d), 1.0);


	
	vec3 color = pow(vColor, vec3(1.0/exposure));

	fragColor = vec4(color * d, 1.0);
}
