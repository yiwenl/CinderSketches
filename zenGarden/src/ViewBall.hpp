//
//  ViewBall.hpp
//  zenGarden
//
//  Created by Yi-Wen LIN on 01/05/2020.
//

#ifndef ViewBall_hpp
#define ViewBall_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViewBall {
public:
    ViewBall() {
        _init();
    }
    
    
    void draw(vec3 position, vec3 scale, vec3 posCam) {
        gl::ScopedGlslProg progColor( mShader );
        mShader->uniform("uPosition", position);
        mShader->uniform("uScale", scale);
        mShader->uniform("uPosCam", posCam);
        
        mBatch->draw();
    }
    
private:
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mShader;
    
    void _init() {
        auto sphere = gl::VboMesh::create( geom::Plane().normal(vec3(0, 0, 1)) );
        mShader = gl::GlslProg::create( gl::GlslProg::Format()
                .vertex( CI_GLSL(100, precision highp float;

                uniform mat4    ciModelViewProjection;
                uniform mat4    ciModelViewInverse;
                uniform mat3    ciNormalMatrix;

                attribute vec4        ciPosition;
                attribute vec2        ciTexCoord0;
                attribute vec3        ciNormal;

                uniform vec3 uPosition;
                uniform vec3 uScale;
                uniform vec3 uPosCam;

                varying highp vec3    Normal;
                varying highp vec2    TexCoord0;

                vec2 rotate(vec2 v, float a) {
                    float s = sin(a);
                    float c = cos(a);
                    mat2 m = mat2(c, s, -s, c);
                    return m * v;
                }

                mat4 rotationMatrix(vec3 axis, float angle) {
                    axis = normalize(axis);
                    float s = sin(angle);
                    float c = cos(angle);
                    float oc = 1.0 - c;
                    
                    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                                0.0,                                0.0,                                0.0,                                1.0);
                }

                vec3 rotate(vec3 v, vec3 axis, float angle) {
                    mat4 m = rotationMatrix(axis, angle);
                    return (m * vec4(v, 1.0)).xyz;
                }


                vec3 align(vec3 pos, vec3 dir, vec3 initDir) {
                    vec3 axis = cross(dir, initDir);
                    float angle = acos(dot(dir, initDir));
                    return rotate(pos, axis, angle);
                }


                void main( void )
                {
                    vec3 pos        = ciPosition.xyz;
                    pos             *= uScale;
                    vec3 dir = normalize(uPosCam - uPosition);
                    pos = align(pos, dir, vec3(0.0, 0.0, 1.0));
                    // pos = vec3(ciModelViewInverse * vec4(pos, 1.0));
            
                    pos             += uPosition;
                    pos.y           += 0.01;
                    
                    gl_Position     = ciModelViewProjection * vec4(pos, 1.0);
                    TexCoord0       = ciTexCoord0;
                    Normal          = ciNormal;
                }))
                .fragment( CI_GLSL(100, precision mediump float;
        
                precision highp float;

                varying vec3 Normal;
                varying vec2 TexCoord0;


                const vec3 LIGHT = vec3(0.0, 0.0, 1.0);

                void main( void )
                {
//                    float d = max(dot(normalize(Normal), normalize(LIGHT)), 0.0);
//                    d = smoothstep(0.1, 1.0, d);
//                    gl_FragColor = vec4( vec3(d), 1.0);

                    float d = distance(TexCoord0, vec2(.5)); 
                    d = smoothstep(0.45, 0.0, d);
                    gl_FragColor = vec4( vec3(d), 1.0);
                    // gl_FragColor = vec4( vec3(1.0), d);

                })));
        mBatch = gl::Batch::create(sphere, mShader);
    }
};

#endif /* ViewBall_hpp */
