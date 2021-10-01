#version 300 es

precision highp float;

uniform mat4    ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec2    uViewport;
uniform float   uOffset;
uniform sampler2D uShadowMap;

in vec4         ciPosition;
in vec3         iPositionOrg;
in vec3         iRandom;
in vec3         iColor;
in float        iLife;

out vec3        vColor;
out vec4        vShadowCoord;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0 );

const float radius = 0.015;

#define uMapSize vec2(1024.0)

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


void main( void )
{
  gl_Position         = ciModelViewProjection * ciPosition;

  float distOffset    = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
  float scale         = mix(1.0, 2.0, iRandom.y);
  gl_PointSize        = distOffset * uOffset * scale;

  float br = smoothstep(0.0, 0.5, iLife);
  br = pow(br, 2.0);
  br = mix(br, 1.0, .5) * 1.2;


  vShadowCoord        = ( biasMatrix * uShadowMatrix ) * ciPosition;

  vec4 sc         = vShadowCoord / vShadowCoord.w;
  float shadow    = PCFShadow(uShadowMap, uMapSize, sc );
  shadow          = mix(shadow, 1.0, .5);
  vColor          = mix(1.5, 1.0, iRandom.z) * iColor * br * shadow;
}
