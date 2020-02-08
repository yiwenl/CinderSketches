//
//  BatchHelpers.hpp
//  Particles002
//
//  Created by Yi-Wen LIN on 08/02/2020.
//

#ifndef BatchHelpers_hpp
#define BatchHelpers_hpp

#include "cinder/gl/gl.h"
#include <stdio.h>

#endif /* BatchHelpers_hpp */

using namespace ci;
using namespace ci::app;
using namespace std;
const int NUM_DOTS = 100;


typedef std::shared_ptr<class BatchAxis> BatchAxisRef;
typedef std::shared_ptr<class BatchGridDots> BatchGridDotsRef;

class BatchAxis {
public:
    BatchAxis() {
        
    }
    static BatchAxisRef create() { return std::make_shared<BatchAxis>(); }
    
    void draw() {
        gl::ScopedColor scp;
        
        gl::lineWidth(2.0f);
        gl::color( ColorA( 1, 0, 0, 1.0f ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( vec3(-1000.0, 0.0, 0.0));
        gl::vertex( vec3( 1000.0, 0.0, 0.0));
        gl::end();
        
        gl::color( ColorA( 0, 1, 0, 1.0f ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( vec3(0.0, -1000.0, 0.0));
        gl::vertex( vec3(0.0,  1000.0, 0.0));
        gl::end();
        
        gl::color( ColorA( 0, 0, 1, 1.0f ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( vec3(0.0, 0.0, -1000.0));
        gl::vertex( vec3(0.0, 0.0,  1000.0));
        gl::end();
    }
};

class BatchGridDots {
public:
    BatchGridDots() {
        _init();
    }
    
    void draw(float gap) {
        gl::ScopedGlslProg progColor( mShader );
        mShader->uniform("uGap", gap);
        mBatch->draw();
    }
    
    void draw() {
        draw(1.0f);
    }
    
    
    
    static BatchGridDotsRef create() { return std::make_shared<BatchGridDots>(); }
    
protected:
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mShader;
    
    void _init() {
        console() << " Init Grid dots " << endl;
        
        vector<vec3> points;
        
        for(int i = 0; i<NUM_DOTS; i++) {
            for(int j = 0; j<NUM_DOTS; j++) {
                points.push_back(vec3(i - NUM_DOTS/2, j - NUM_DOTS/2, 0.0f));
                points.push_back(vec3(i - NUM_DOTS/2, 0.0f, j - NUM_DOTS/2));
            }
        }
        
        
        gl::VboRef mParticleVbo = gl::Vbo::create( GL_ARRAY_BUFFER, points, GL_STREAM_DRAW );
        
        // Describe particle semantics for GPU.
        geom::BufferLayout particleLayout;
        particleLayout.append( geom::Attrib::POSITION, 3, sizeof( vec3 ), 0);
        
        auto mesh = gl::VboMesh::create( int(points.size()), GL_POINTS, { { particleLayout, mParticleVbo } } );
        
        mShader = gl::GlslProg::create( gl::GlslProg::Format()
                .vertex( CI_GLSL(100, precision highp float;

                uniform mat4        ciModelViewProjection;
                uniform float       uGap;
                attribute vec4      ciPosition;


                void main( void )
                {
                    vec4 pos       = ciPosition;
                    pos.xyz        *= uGap;
                    gl_Position    = ciModelViewProjection * pos;
                    gl_PointSize   = 1.0;
                }))
                .fragment( CI_GLSL(100, precision mediump float;
        
                void main( void )
                {
                    gl_FragColor = vec4( 1.0);
                })));
        mBatch = gl::Batch::create(mesh, mShader);
    
    }
};
