#version 300 es

precision highp float;

#define uMapSize vec2(1024.0)

in vec4         vShadowCoord;
in vec3         vColor;

uniform sampler2D uShadowMap;

out highp vec4  oColor;



float rand(vec4 seed4) {
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
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


void main( void ) {
    vec4 sc         = vShadowCoord / vShadowCoord.w;
//    float shadow    = PCFShadow(uShadowMap, uMapSize, sc );
    // shadow          = mix(shadow, 1.0, .4);
    
    float d = texture(uShadowMap, sc.xy).r;
    float s = d > sc.z - 0.005 ? 1.0 : 0.0;
    d = s < 1.0 ? 0.0 : 1.0;
    
    if(d >= 1.0) {
//        discard;
    }
    
    oColor = vec4(vec3(0.0), 1.0 - s);
//    oColor = vec4(vec3(1.0 - s), 1.0);
    oColor = vec4(vec3(d), 1.0);
//    oColor = vec4(sc.xy, 0.0, 1.0);
//    oColor = vec4(vec3(shadow), 1.0);
}
