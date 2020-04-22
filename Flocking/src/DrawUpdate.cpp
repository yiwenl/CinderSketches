//
//  DrawUpdate.cpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#include "DrawUpdate.hpp"
#include "Config.hpp"

void DrawUpdate::_init() { 
  console() << "Init draw update" << endl;
  mTexTest = gl::Texture2d::create( loadImage( loadAsset( "c001.jpg" )));
    
  mShader = gl::GlslProg::create(gl::GlslProg::Format()
        .vertex( loadAsset( "update.vert" ) )
        .fragment( loadAsset("update.frag") )
    );
    
    auto plane = gl::VboMesh::create( geom::Plane() );
    mBatch = gl::Batch::create(plane, mShader);
    
}

void DrawUpdate::render(FboPingPongRef mFbo) { 
    gl::ScopedFramebuffer fbo( mFbo->write() );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->write()->getSize() );
    gl::ScopedMatrices matScope;
    
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedGlslProg glslScope( mShader );
    gl::ScopedTextureBind tex0( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT0), (uint8_t) 0 );
    mShader->uniform( "uTexPos", 0 );
    
    gl::ScopedTextureBind tex1( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT1), (uint8_t) 1 );
    mShader->uniform( "uTexVel", 1 );
    
    gl::ScopedTextureBind tex2( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT2), (uint8_t) 2 );
    mShader->uniform( "uTexData", 2 );
    
    gl::ScopedTextureBind tex3( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT3), (uint8_t) 3 );
    mShader->uniform( "uTexExtra", 3 );
    
    mShader->uniform( "uTime", (float)getElapsedSeconds() * 0.1f );
    mShader->uniform( "uNum", Config::getInstance().NUM_PARTICLES );
    
    mBatch->draw();
}
