//
//  DrawParticles.cpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#include "DrawParticles.hpp"
#include "Config.hpp"

void DrawParticles::_init() {
    int NUM_PARTICLES = Config::getInstance().NUM_PARTICLES;
        
    float num = float(NUM_PARTICLES);
    vector<vec2> uvParticle;
    
    for(int i=0; i<NUM_PARTICLES; i++) {
        for(int j=0; j<NUM_PARTICLES; j++) {
            
            uvParticle.push_back(vec2(i/num, j/num));
        }
    }
    
    
    gl::VboRef vboUVParticle = gl::Vbo::create(GL_ARRAY_BUFFER, uvParticle);
        
    
    mVao = gl::Vao::create();
    gl::ScopedVao vaoParticle(mVao);
    
    gl::ScopedBuffer vbo3(vboUVParticle);
    gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    gl::enableVertexAttribArray(0);
    
    mShaderRender = gl::GlslProg::create(gl::GlslProg::Format()
        .vertex( loadAsset( "render.vert" ) )
        .fragment( loadAsset("render.frag") )
        .attribLocation("inUV", 0)
    );
}


void DrawParticles::render(gl::FboRef mFbo) {
    int NUM_PARTICLES = Config::getInstance().NUM_PARTICLES;
    
    gl::ScopedGlslProg glsl(mShaderRender);
    gl::ScopedVao vao(mVao);
    gl::setDefaultShaderVars();
    gl::ScopedTextureBind texScope0( mFbo->getTexture2d(GL_COLOR_ATTACHMENT0), (uint8_t) 0 );
    mShaderRender->uniform( "texturePos", 0 );
    
    gl::ScopedTextureBind texScope1( mFbo->getTexture2d(GL_COLOR_ATTACHMENT2), (uint8_t) 1 );
    mShaderRender->uniform( "textureData", 1 );
    
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    
    gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES * NUM_PARTICLES);
}
