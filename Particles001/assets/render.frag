#version 150 core

in vec3     color;
in vec4     vShadowCoord;

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

void main( void )
{
    if(distance(gl_PointCoord, vec2(.5)) > .5) {
        discard;
    }
    
    vec4 sc    = vShadowCoord / vShadowCoord.w;
    float shadow        = samplePCF4x4( sc );
    // float shadow        = samplePCF3x3( sc );
    
    shadow = mix(shadow, 1.0, .25);
    
    vec4 finalColor = texture(uParticleMap, gl_PointCoord);
    finalColor.rgb *= shadow * color;
    
    
    oColor = finalColor;
//    oColor = vec4(gl_PointCoord, 0.0, 1.0);
}
