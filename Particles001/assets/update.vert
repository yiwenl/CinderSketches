#version 150 core

/*
 .feedbackVaryings( { "position", "positionOrg", "life"} )
 .attribLocation( "iPosition", 0 )
 .attribLocation( "iPositionOrg", 1 )
 .attribLocation( "iLife", 2 )
 */

in vec3   iPosition;
in vec3   iPositionOrg;
in vec3   iRandom;
in float  iLife;

out vec3  position;
out vec3  positionOrg;
out vec3  random;
out float life;



void main()
{
    position =  iPosition;
    positionOrg = iPositionOrg;
    random = iRandom;
    
    
    vec2 dir = normalize(position.xy);
    position.xy += dir * 0.01 * mix(1.0, 2.0, random.y);
    
    
    life = iLife - mix(0.02, 0.1, random.x);
    
    if(life < 0.0f) {
        life = 1.0;
        position = iPositionOrg;
    }
}
