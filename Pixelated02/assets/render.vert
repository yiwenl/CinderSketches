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
in vec3            iColor;
in vec3            iExtra;

out vec3           vColor;
out vec4           vShadowCoord;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0 );

const float radius = 0.002;


void main( void )
{
	gl_Position         = ciModelViewProjection * ciPosition;
    
    float distOffset    = uViewport.y * ciProjectionMatrix[1][1] * radius / gl_Position.w;
    float scale         = mix(1.0, 2.0, iExtra.y);
    gl_PointSize        = distOffset * scale;
    
    float br = 1.0 + smoothstep(0.5, 1.0, iExtra.z) * 4.0;
    
    vColor              = iColor;
    vShadowCoord        = ( biasMatrix * uShadowMatrix ) * ciPosition;
    // vColor = iExtra;
}
