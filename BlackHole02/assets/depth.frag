#version 300 es

precision highp float;
out highp vec4  oColor;

void main( void ) {
  float d = gl_FragCoord.z / gl_FragCoord.w;    
  oColor = vec4(vec3(d), 1.0);
  oColor = vec4(1.0);
}
