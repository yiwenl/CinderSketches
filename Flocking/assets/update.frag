#version 330 core
#include "./fragments/curlNoise.glsl"
#include "./fragments/map.glsl"

in vec2 vUV;

uniform sampler2D uTexPos;
uniform sampler2D uTexVel;
uniform sampler2D uTexData;
uniform sampler2D uTexExtra;
uniform float uTime;
uniform int uNum;

layout (location = 0) out vec4 oFragColor0;
layout (location = 1) out vec4 oFragColor1;
layout (location = 2) out vec4 oFragColor2;
layout (location = 3) out vec4 oFragColor3;

// int NUM = 80;

const float maxRadius = 12.0;
const float maxSenseRadius = 2.0;
const float minThreshold = 0.2;
const float maxThreshold = 0.6;
const float maxSpeed = 0.02;
const float flashEasing = 0.0005;

#define PI 3.141592653
#define PI2 PI * 2.0

void main( void )
{
    vec3 pos = texture(uTexPos, vUV).xyz;
    vec3 vel = texture(uTexVel, vUV).xyz;
    vec3 data = texture(uTexData, vUV).xyz;
    vec3 extra = texture(uTexExtra, vUV).xyz;
    float cycle = data.r;

    float dist, f, delta, p, t;
    vec3 dir;
    vec3 dirAlignment = vec3(0.0);
    float numNeighbors = 0.0;

    vec3 acc = vec3(0.0);

    // flocking
    vec2 uvParticles = vec2(0.0);
    vec3 posParticle, velParticle, dataParticle;
    float cycleParticle, d0, d1, d;
    float num = 80.0;
    float senseRadius = mix(1.0, 1.5, extra.b) * maxSenseRadius;

    

    for(float i=0.0; i<uNum; i++) {
        for(float j=0.0; j<uNum; j++) {
            uvParticles = vec2(i/num, j/num);
            posParticle = texture(uTexPos, uvParticles).xyz;
            velParticle = texture(uTexVel, uvParticles).xyz;
            dataParticle = texture(uTexData, uvParticles).xyz;

            dist = distance(pos, posParticle);
            if(dist > 0.0 && dist < senseRadius) {
                p = dist / senseRadius;
                if(p < minThreshold) {
                    delta = map(p, 0.0, minThreshold);
                    dir = normalize(pos - posParticle);
                    f = 1.0 / delta;
                    f = min(f, 5.0);
                    acc += dir * 0.005 * f;
                } else if( p > maxThreshold) {
                    delta = map(p, maxThreshold, 1.0);
                    delta = sin(delta * PI);
                    dir = normalize(posParticle - pos);
                    f = pow(delta, 1.5) * 0.02;
                    acc += dir * 0.003 * f;
                }

                // alignment
                dir = (normalize(vel) + normalize(velParticle)) * 0.5;
                // dir = (normalize(velParticle) - normalize(vel) );
                // dir = normalize(dir);
                f = sin(p * PI);
                acc += dir * f * 0.0005;

                numNeighbors += 1.0;
                // dirAlignment += normalize(velParticle) * f;

                // flash sync - Entrainment
                cycleParticle = dataParticle.x;

                if(cycleParticle > cycle) {
                    d0 = cycleParticle - cycle;
                    d1 = cycle + PI2 - cycleParticle;
                    d = d0 < d1 ? d0 : -d1;
                } else {
                    d0 = cycle - cycleParticle;
                    d1 = cycleParticle + PI2 - cycle;
                    d = d0 < d1 ? -d0 : d1;
                }

                if(dist < senseRadius * 0.5) {
                    cycle += d * flashEasing * mix(0.5, 1.0, extra.r);
                }
                
            } 
        }
    }

    // if(numNeighbors > 0.0) {
    //     dirAlignment /= numNeighbors;
    //     acc += dirAlignment * .1;
    // }

    data.z = numNeighbors;


    // noise
    float posOffset = snoise(pos * 0.1 + extra * 5.0 + uTime * 0.5) * .5 + .5;
    posOffset = mix(0.1, 0.2, posOffset);
    vec3 noise = curlNoise(pos * posOffset + uTime);
    acc += noise * 0.1;

    // pull back in
    vec3 center = vec3(8.0);
    dist = distance(pos, center);
    f = smoothstep(maxRadius * 0.25, maxRadius, dist);
    dir = normalize(pos - center);
    acc -= dir * f;
    
    t = mix(0.25, 1.0, extra.g) * uTime + extra.b;
    t = sin(t) * .5 + .5;
    float speedOffset = mix(0.5, 1.0, t);
    vel += acc * 0.0005 * speedOffset;

    if(length(vel) > maxSpeed) {
        vel = normalize(vel) * maxSpeed;
    }

    pos += vel;

    vel *= 0.97;


    float cycleSpeed = mix(0.25, 1.0, data.y) * 0.05;
    cycle += cycleSpeed;
    data.x = mod(cycle, PI2);


    oFragColor0 = vec4(pos, 1.0);
    oFragColor1 = vec4(vel, 1.0);
    oFragColor2 = vec4(data, 1.0);
    oFragColor3 = vec4(extra, 1.0);
}
