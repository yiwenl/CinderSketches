#version 150 core

in vec3     vColor;
in vec4     vShadowCoord;
in float    vLife;


out highp vec4     oColor;

uniform sampler2DShadow uShadowMap;
uniform sampler2D uParticleMap;

float samplePCF3x3( vec4 sc )
{
    const int s = 2;
    
    float shadow = 0.0;
    shadow += textureProjOffset( uShadowMap, sc, ivec2(-s,-s) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2(-s, 0) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2(-s, s) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2( 0,-s) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2( 0, 0) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2( 0, s) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2( s,-s) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2( s, 0) );
    shadow += textureProjOffset( uShadowMap, sc, ivec2( s, s) );
    return shadow/9.0;;
}

float samplePCF4x4( vec4 sc )
{
    const int r = 2;
    const int s = 2 * r;
    
    float shadow = 0.0;
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s,-s) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r,-s) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r,-s) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s,-s) );
    
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s,-r) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r,-r) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r,-r) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s,-r) );
    
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s, r) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r, r) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r, r) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s, r) );
    
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-s, s) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2(-r, s) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( r, s) );
    shadow += textureProjOffset( uShadowMap,  sc, ivec2( s, s) );
        
    return shadow/16.0;
}


const float brOffset = 1.2;

void main( void )
{
    if(distance(gl_PointCoord, vec2(.5)) > .45) {
        discard;
    }
    
    vec4 sc         = vShadowCoord / vShadowCoord.w;
    float shadow    = samplePCF4x4( sc );
    shadow          = mix(shadow, 1.0, .1);
    
    vec3 color = texture(uParticleMap, gl_PointCoord).rgb;
    
    color *= shadow;
    
    oColor = vec4(color, 1.0);
}
