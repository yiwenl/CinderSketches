#version 300 es

precision highp float;

in vec3    vExtra;
in vec3    Normal;
in vec2    TexCoord0;


uniform vec3 uColor;

out highp vec4  oColor;


#define GREEN vec3(0.5, 0.9, 0.5) * 0.85

void main( void ) {
//    oColor = vec4(TexCoord0, 0.0, 1.0);
    
    // float g = mix(vExtra.z, 1.0, .25) * 0.5;
    // oColor = vec4(vec3(g), 0.75);

    float a = mix(0.5, 0.75, vExtra.y);
    oColor = vec4(uColor * mix(0.5, 1.0, vExtra.z), a);
}
