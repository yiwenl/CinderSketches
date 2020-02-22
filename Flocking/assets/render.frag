#version 150 core

out highp vec4     oColor;

void main( void )
{
    if(distance(gl_PointCoord, vec2(.5)) > .45) {
        discard;
    }

    oColor = vec4(1.0);
}
