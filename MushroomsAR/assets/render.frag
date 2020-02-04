#version 300 es


precision highp float;

#define uMapSize vec2(2048.0)

in vec4     vShadowCoord;
in vec3     vColor;
in float    vLife;


out highp vec4     oColor;


//uniform sampler2D uShadowMap;
//uniform sampler2D uParticleMap;



void main( void )
{
    if(distance(gl_PointCoord, vec2(.5)) > .45) {
        discard;
    }
    
    oColor = vec4(vColor, 1.0);
}
