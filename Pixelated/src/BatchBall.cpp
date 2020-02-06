//
//  BatchBall.cpp
//  Pixelated
//
//  Created by Yi-Wen LIN on 06/02/2020.
//

#include "BatchBall.hpp"

BatchBall::BatchBall(){
    console() << "Init Batch Ball" << endl;
    
    _init();
}


void BatchBall::_init() {
    auto sphere = gl::VboMesh::create( geom::Sphere() );
    mShader = gl::GlslProg::create( gl::GlslProg::Format()
            .vertex( CI_GLSL(100, precision mediump float;

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


void BatchBall::draw(vec3 position, vec3 scale, vec3 color, float opacity) {
    gl::ScopedGlslProg progColor( mShader );
    mShader->uniform("uPosition", position);
    mShader->uniform("uScale", scale);
    mShader->uniform("uColor", color);
    mShader->uniform("uOpacity", opacity);
    
    mBatch->draw();
}


void BatchBall::draw(vec3 position, vec3 scale, vec3 color) {
    draw(position, scale, color, 1.0);
}

void BatchBall::draw(vec3 position, vec3 scale) {
    draw(position, scale, vec3(1.0), 1.0);
}

void BatchBall::draw(vec3 position) {
    draw(position, vec3(1.0), vec3(1.0), 1.0);
}

