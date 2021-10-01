#version 300 es

precision highp float;
in vec4         vScreenPos;
in vec4         vScreenPos2;
in vec4         vScreenPos3;
in vec3         vNormal;
in vec3         vExtra;
in vec3         vPosition;
in vec2         vUV;

out highp vec4  oColor;


uniform sampler2D uEnvMap;
uniform float   uOffset;
uniform float   uTime;

// #include "./fragments/curlNoise.glsl"

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}
float snoise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


void main( void ) {
    vec2 uv = (vScreenPos.xy / vScreenPos.w) * .5 + .5;
    vec4 color = texture(uEnvMap, uv);

    uv = (vScreenPos2.xy / vScreenPos2.w) * .5 + .5;
    vec4 color2 = texture(uEnvMap, uv);

    uv = (vScreenPos3.xy / vScreenPos3.w) * .5 + .5;
    vec4 color3 = texture(uEnvMap, uv);

    float p = vExtra.x * vExtra.y + .2;
    float strength = 0.25;
    color.rgb = mix(color.rgb, color2.rgb, p * uOffset);
    color.rgb = mix(color.rgb, color3.rgb, p * uOffset);
    color.rgb += color2.rgb * vExtra.y * strength * uOffset;
    color.rgb += color3.rgb * vExtra.z * strength * uOffset;

    float br = 1.0 - vExtra.x;
    br = pow(br, 2.0);
    br = mix(1.0, br, uOffset);
    color *= br;

    vec2 scale = vec2(1.0);
    float t = 100.0;
    if(abs(vPosition.x) < abs(vPosition.y)) {
        scale = vec2(t, 1.0);
    } else {
        scale = vec2(1.0, t);
    }
    float noise = snoise(vUV * scale + uTime * 5.0);

    const float THRESHOLD = 0.5;
    float dx = abs(vPosition.x) / 3.0;
    dx = smoothstep(THRESHOLD + noise * 0.1, 1.0, dx);

    float dy = abs(vPosition.y) / 3.0;
    dy = smoothstep(THRESHOLD + noise * 0.1, 1.0, dy);

    float d = dx + dy;
    d = 1.0 - pow(d, 1.5);
    d = mix(1.0, d, 0.75);

    color.rgb *= d;

    oColor = color;
    // oColor = vec4(vec3(d), 1.0);
}
