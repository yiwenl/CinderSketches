uniform vec2    uViewport;
uniform mat4    ciProjectionMatrix;

float pointSize(vec4 screenPos, float radius) {
    return uViewport.y * ciProjectionMatrix[1][1] * radius / screenPos.w;
}
