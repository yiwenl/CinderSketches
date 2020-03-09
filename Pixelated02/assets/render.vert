#version 300 es

uniform mat4    ciModelViewProjection;
uniform mat4    ciProjectionMatrix;
uniform mat4    ciViewMatrix;
uniform mat4    ciModelMatrix;
uniform mat4    uShadowMatrix;

uniform vec2       uViewport;

in vec4            ciPosition;
in vec3            iPositionOrg;
in vec3            iVel;
in vec3            iExtra;

out vec3           vColor;

const float radius = 0.001;


void main( void )
{
	gl_Position         = ciModelViewProjection * ciPosition;
    
    float distOffset    = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale         = mix(1.0, 2.0, iExtra.y);
    gl_PointSize        = distOffset * scale;
    // gl_PointSize = 5.0;
    
    
    vColor              = iExtra;
}
