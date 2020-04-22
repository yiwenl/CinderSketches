//
//  BatchPlane.hpp
//  RayCasting
//
//  Created by Yi-Wen LIN on 21/04/2020.
//

#ifndef BatchPlane_hpp
#define BatchPlane_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::shared_ptr<class BatchPlane> BatchPlaneRef;

class BatchPlane {
public:
    
    static BatchPlaneRef create() { return std::make_shared<BatchPlane>(); }
    
    BatchPlane() {
        _init();
    }
    
    
    void draw() {
        draw(vec3(0.0), vec3(1.0), vec3(1.0), 1.0);
    }
    
    void draw(vec3 position) {
        draw(position, vec3(1.0), vec3(1.0), 1.0);
    }
    
    
    void draw(vec3 position, vec3 scale) {
        draw(position, scale, vec3(1.0), 1.0);
    }
    
    
    void draw(vec3 position, vec3 scale, vec3 color) {
        draw(position, scale, color, 1.0);
    }
    
    
    void draw(vec3 position, vec3 scale, vec3 color, float opacity) {
        gl::ScopedGlslProg progColor( mShader );
        mShader->uniform("uPosition", position);
        mShader->uniform("uScale", scale);
        mShader->uniform("uColor", color);
        mShader->uniform("uOpacity", opacity);
        
        mBatch->draw();
    }
    
protected:
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mShader;
    
    // methods
    
    void _init() {
        console() << "init batch plane" << endl;
        
        auto plane = gl::VboMesh::create( geom::Plane() );
        
        mShader = gl::GlslProg::create( gl::GlslProg::Format()
                .vertex( CI_GLSL(100, precision highp float;

                uniform mat4    ciModelViewProjection;
                uniform mat3    ciNormalMatrix;

                attribute vec4        ciPosition;
                attribute vec2        ciTexCoord0;
                attribute vec3        ciNormal;

                uniform vec3 uPosition;
                uniform vec3 uScale;

                varying highp vec3    Normal;
                varying highp vec2    TexCoord0;

                void main( void )
                {
                    vec4 pos        = ciPosition;
                    pos.xyz         *= uScale;
                    pos.xyz         += uPosition;
                    
                    gl_Position     = ciModelViewProjection * pos;
                    TexCoord0       = ciTexCoord0;
                    Normal          = ciNormalMatrix * ciNormal;
                }))
                .fragment( CI_GLSL(100, precision mediump float;
        
                precision highp float;

                varying vec3 Normal;

                uniform vec3 uColor;
                uniform float uOpacity;

                void main( void )
                {
                    gl_FragColor = vec4( uColor, uOpacity);
                })));
        mBatch = gl::Batch::create(plane, mShader);
    }
};




#endif /* BatchPlane_hpp */
