#version 300 es


precision highp float;

#define uMapSize vec2(2048.0)

in vec3     color;
in vec4     vShadowCoord;
in vec4     vScreenCoord;
in float    vLife;


out highp vec4     oColor;

uniform sampler2D uShadowMap;
uniform sampler2D uParticleMap;
uniform sampler2D uEnvMap;

float rand(vec4 seed4) {
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}


float samplePCF4x4( vec4 sc )
{
    const int r = 2;
    const int s = 2 * r;
    
    float shadow = 0.0;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s,-s) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r,-s) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r,-s) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s,-s) ).r;
    
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s,-r) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r,-r) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r,-r) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s,-r) ).r;
    
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s, r) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r, r) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r, r) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s, r) ).r;
    
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s, s) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r, s) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r, s) ).r;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s, s) ).r;
        
    return shadow/16.0;
}


float PCFShadow(sampler2D depths, vec2 size, vec4 shadowCoord) {
	float result = 0.0;
	float bias = 0.005;
    float count = 0.0;
	vec2 uv = shadowCoord.xy;

    const int range = 2;

	for(int x=-range; x<range; x++){
		for(int y=-range; y<range; y++){
			vec2 off = vec2(x,y);
			off /= size;

			float d = texture(depths, uv + off).r;
			if(d < shadowCoord.z - bias) {
				result += 1.0;
			}

            count += 1.0;
		}
	}
	return 1.0 -result/count;

}


float luma(vec3 color) {
  return dot(color, vec3(0.299, 0.587, 0.114));
}

const float brOffset = 1.5;

void main( void )
{
    if(distance(gl_PointCoord, vec2(.5)) > .45) {
        discard;
    }
    
    // particle texture
    vec2 uv = gl_PointCoord;
    uv.y = 1.0 - uv.y;
    vec4 finalColor = texture(uParticleMap, uv);
    

    // shadow mapping
    vec4 sc    = vShadowCoord / vShadowCoord.w;
    float shadow    = PCFShadow(uShadowMap, uMapSize, sc);
	shadow          = mix(shadow, 1.0, .5);

    // background
    vec2 screenUV = vScreenCoord.xy / vScreenCoord.w * .5 + .5;
    vec4 colorEnv = texture(uEnvMap, screenUV);

    // color composite
    finalColor.rgb *= shadow * brOffset * colorEnv.rgb;


    oColor = finalColor;

}
