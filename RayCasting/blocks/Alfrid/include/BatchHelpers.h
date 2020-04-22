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

using namespace ci;
using namespace ci::app;
using namespace std;
const int NUM_DOTS = 100;


typedef std::shared_ptr<class BatchAxis> BatchAxisRef;
typedef std::shared_ptr<class BatchGridDots> BatchGridDotsRef;
typedef std::shared_ptr<class BatchBall> BatchBallRef;
typedef std::shared_ptr<class BatchPlane> BatchPlaneRef;
typedef std::shared_ptr<class EaseNumber> EaseNumberRef;

class AlfridUtils {
public:
    static vec3 getLookDir(mat4 mtxView) {
        vec3 front = vec3(0, 0, 1);
        mat3 mtxRot = mat3(mtxView);
        mtxRot = glm::transpose(mtxRot);
        front = mtxRot * front;
        front *= -1.0;
        
        return front;
    }
};

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
                    gl_PointSize   = 2.0;
                }))
                .fragment( CI_GLSL(100, precision mediump float;
        
                void main( void )
                {
                    gl_FragColor = vec4( 1.0, 1.0, 1.0, 0.5);
                })));
        mBatch = gl::Batch::create(mesh, mShader);
    
    }
};


class BatchBall {
    
public:
    BatchBall() {
        _init();
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
    
    static BatchBallRef create() { return std::make_shared<BatchBall>(); }
    
protected:
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mShader;
    
    void _init() {
        auto sphere = gl::VboMesh::create( geom::Sphere() );
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
                    vec4 pos         = ciPosition;
                    pos.xyz         *= uScale;
                    pos.xyz         += uPosition;
                    
                    gl_Position    = ciModelViewProjection * pos;
                    TexCoord0     = ciTexCoord0;
                    Normal            = ciNormalMatrix * ciNormal;
                }))
                .fragment( CI_GLSL(100, precision mediump float;
        
                precision highp float;

                varying vec3 Normal;


                uniform vec3 uColor;
                uniform float uOpacity;

                const vec3 LIGHT = vec3(0.2, 1.0, 0.6);

                void main( void )
                {
                    float d = max(dot(normalize(Normal), normalize(LIGHT)), 0.0);
                    d = mix(d, 1.0, .25);
                    gl_FragColor = vec4( uColor * d, uOpacity);
                })));
        mBatch = gl::Batch::create(sphere, mShader);
    }
};


class EaseNumber {
private:
    float _value;
    float _target;
    
public:
    float easing = 0.1;
    
    EaseNumber(float mValue) {
        setTo(mValue);
    }
    
    
    void update() {
        _value += (_target - _value) * easing;
    }
    
    void setTo(float mValue) {
        _target = _value = mValue;
    }
    
    
    float getValue() {
        return _value;
    }
    
    void setValue(float mValue) {
        _target = mValue;
    }
    
    
    static EaseNumberRef create(float mValue) { return std::make_shared<EaseNumber>(mValue); }

};



class BatchLine {
public:
    
    BatchLine() {
        
    }
    
    static void draw(vec3 mPointA, vec3 mPointB) {
        draw(mPointA, mPointB, vec3(1.0), 1.0f);
    }
    
    static void draw(vec3 mPointA, vec3 mPointB, vec3 mColor) {
        draw(mPointA, mPointB, mColor, 1.0f);
    }
    
    static void draw(vec3 mPointA, vec3 mPointB, vec3 mColor, float mOpacity) {
        gl::ScopedColor scp;
        
        gl::lineWidth(2.0f);
        gl::color( ColorA( mColor.x, mColor.y, mColor.z, mOpacity ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( mPointA );
        gl::vertex( mPointB );
        gl::end();
        
    }
    
};


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



#endif /* BatchHelpers_hpp */
