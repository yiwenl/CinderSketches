
in vec2	vTexCoord0;


uniform sampler2D texture;
uniform sampler2D uTexColor;
uniform sampler2D uTexDepth;
uniform float uNear;
uniform float uFar;
uniform float uRatio;

out vec4 fragColor;

float scale = 0.25;
float textureWidth = 2048.0 * scale;
float textureHeight = 2048.0 * scale;

const float PI = 3.141592657;

const int samples = 16; //samples on the first ring (4-8)
const int rings = 9; //ring count (3-6)


vec2 rand(in vec2 coord) //generating random noise
{
	float noiseX = (fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453));
	float noiseY = (fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453));
	return vec2(noiseX,noiseY) * 0.001;
}

float readDepth(in vec2 coord)
{
	return (2.0 * uNear) / (uFar + uNear - texture2D(uTexDepth, coord ).x * (uFar-uNear));        
}


float compareDepths( in float depth1, in float depth2 )
{
        float aoCap = 1.0;
        float aoMultiplier = 50.0;
        float depthTolerance = 0.000;
        float aorange = 50.0;// units in space the AO effect extends to (this gets divided by the camera far range
        float diff = sqrt(clamp(1.0-(depth1-depth2) / (aorange/(uFar-uNear)),0.0,1.0));
        float ao = min(aoCap,max(0.0,depth1-depth2-depthTolerance) * aoMultiplier) * diff;
        return ao;
}

float ssao() {
    vec2 vTextureCoord = vTexCoord0;
	float depth = readDepth(vTextureCoord);
	float d;
	float aspect = textureWidth/textureHeight;
	vec2 noise = rand(vTextureCoord * 10.0);

	float w = (1.0 / textureWidth)/clamp(depth,0.05,1.0)+(noise.x*(1.0-noise.x));
    float h = (1.0 / textureHeight)/clamp(depth,0.05,1.0)+(noise.y*(1.0-noise.y));
   
    float pw = 0.0;
    float ph = 0.0;

    float ao = 0.0;       
    float s = 0.0;
    float fade = 4.0;
    float t = 1.0;


    for (int i = 0 ; i < rings; i += 1) {
    	fade *= 0.25;
        for (int j = 0 ; j < samples*rings; j += 1) {
        	if (j >= samples*i) break;
            float step = PI*2.0 / (float(samples)*float(i));
            pw = (cos(float(j)*step)*float(i)) * t;
            ph = (sin(float(j)*step)*float(i))*aspect * t;
            d = readDepth( vec2(vTextureCoord.s+pw*w,vTextureCoord.t+ph*h));
            ao += compareDepths(depth,d)*fade;     
            s += 1.0*fade;
        }
    }
   
    ao /= s;
    ao = 1.0 - ao;
    float offset = .5;
    ao = offset + (1.0 - offset) * ao;
    ao = pow(ao, 2.0);

	return ao;
}

void main()
{	
    float ao = ssao();
    ao = mix(ao, 1.0, .65);
    vec3 color = texture2D(uTexColor, vTexCoord0).rgb;
    color *= ao;
	fragColor = vec4(color, 1.0);
}
