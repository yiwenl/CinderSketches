#version 150 core

in vec3     color;
in vec4     vShadowCoord;
in vec4     vScreenCoord;
in float    vLife;


out highp vec4     oColor;

uniform sampler2DShadow uShadowMap;
uniform sampler2D uParticleMap;
uniform sampler2D uEnvMap;



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
    if(distance(gl_PointCoord, vec2(.5)) > .5) {
        discard;
    }
    
    vec4 sc    = vShadowCoord / vShadowCoord.w;
    float shadow        = samplePCF4x4( sc );
    // float shadow        = samplePCF3x3( sc );
    
    shadow = mix(shadow, 1.0, .1);
    vec2 screenUV = vScreenCoord.xy / vScreenCoord.w * .5 + .5;
    vec4 colorEnv = texture(uEnvMap, screenUV);
    
    vec4 finalColor = texture(uParticleMap, gl_PointCoord);
    finalColor.rgb *= shadow * color * colorEnv.rgb * brOffset;
    // finalColor.rgb *= shadow * color * brOffset;

    // float opacity = smoothstep(1.0, 0.8, vLife);
    // finalColor.a *= opacity;
    
    oColor = finalColor;
}
