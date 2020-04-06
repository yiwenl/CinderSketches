//
//  DrawSave.cpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#include "DrawSave.hpp"
#include "Config.hpp"
#include "cinder/Rand.h"

void DrawSave::draw(gl::FboRef mFbo) {
    
    int NUM_PARTICLES = Config::getInstance().NUM_PARTICLES;
    vector<vec3> positions;
    vector<vec2> uvs;
    vector<vec3> extras;
    float num = float(NUM_PARTICLES);
    
    for(int i=0; i<NUM_PARTICLES; i++) {
        for(int j=0; j<NUM_PARTICLES; j++) {
            vec3 p = randVec3() * randFloat();
            
            positions.push_back(p);
            float u = i/num * 2.0f - 1.0f;
            float v = j/num * 2.0f - 1.0f;
            uvs.push_back(vec2(u, v));
            extras.push_back(randVec3());
        }
    }
    
    
    gl::VboRef vboPos = gl::Vbo::create(GL_ARRAY_BUFFER, positions);
    gl::VboRef vboUv = gl::Vbo::create(GL_ARRAY_BUFFER, uvs);
    gl::VboRef vboExtra = gl::Vbo::create(GL_ARRAY_BUFFER, extras);
        
    gl::VaoRef mVao = gl::Vao::create();
    gl::ScopedVao vao(mVao);
    
    gl::ScopedBuffer vbo0(vboPos);
    gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    gl::enableVertexAttribArray(0);
    
    gl::ScopedBuffer vbo1(vboUv);
    gl::vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    gl::enableVertexAttribArray(1);
    
    gl::ScopedBuffer vbo2(vboExtra);
    gl::vertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    gl::enableVertexAttribArray(2);
    
    
    gl::GlslProgRef mShaderSave = gl::GlslProg::create(gl::GlslProg::Format()
        .vertex( loadAsset( "save.vert" ) )
        .fragment( loadAsset("save.frag") )
        .attribLocation("inPosition", 0)
        .attribLocation("inUV", 1)
        .attribLocation("inExtra", 2)
    );
    
    gl::ScopedGlslProg glsl(mShaderSave);
    gl::setDefaultShaderVars();

    gl::ScopedFramebuffer fbo(mFbo);
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::drawArrays(GL_POINTS, 0, positions.size());
}
