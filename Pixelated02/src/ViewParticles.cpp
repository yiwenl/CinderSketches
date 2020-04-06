//
//  ViewParticles.cpp
//  Pixelated02
//
//  Created by Yi-Wen LIN on 09/03/2020.
//

#include "ViewParticles.hpp"
#include "cinder/Rand.h"


struct Particle
{
    vec3 pos;
    vec3 posOrg;
    vec3 vel;
    vec3 color;
    vec3 extra;
};

void ViewParticles::init() {
    console() << " Init :" << pos << ", " << NUM_PARTICLES << endl;
    
    
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );
    float range = 0.1;
    float range_z = 0.01;
    
    for( int i =0; i<particles.size(); i++) {
        float a = randFloat(M_PI * 2.0);
        float r = sqrt(randFloat()) * range;
        float x = cos(a) * r;
        float y = sin(a) * r;
        float z = randFloat(-range_z, range_z);
        
        auto &p = particles.at( i );
        
        p.pos = vec3(x, y, z);
        p.posOrg = vec3(x, y, z);
        p.vel = vec3(0, 0, 0);
        p.color = randVec3();
        p.extra = vec3(0, randFloat(), randFloat());
    }
    
    
    mParticleBuffer[mSourceIndex]       = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW );
    mParticleBuffer[mDestinationIndex]  = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW );
    
    
    for( int i = 0; i < 2; ++i )
    {    // Describe the particle layout for OpenGL.
        mAttributes[i] = gl::Vao::create();
        gl::ScopedVao vao( mAttributes[i] );
        
        // Define attributes as offsets into the bound particle buffer
        gl::ScopedBuffer buffer( mParticleBuffer[i] );
        gl::enableVertexAttribArray( 0 );
        gl::enableVertexAttribArray( 1 );
        gl::enableVertexAttribArray( 2 );
        gl::enableVertexAttribArray( 3 );
        gl::enableVertexAttribArray( 4 );
        gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
        gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, posOrg) );
        gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel) );
        gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color) );
        gl::vertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, extra) );
    }
    
    
    // init shaders
    mShaderRender = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vert" ) ).fragment( loadAsset("render.frag"))
       .attribLocation( "ciPosition", 0 )
       .attribLocation( "iPositionOrg", 1 )
       .attribLocation( "iVel", 2 )
       .attribLocation( "iColor", 3 )
       .attribLocation( "iExtra", 4 )
    );
    
    mShaderInit = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "init.vert" ) ).fragment( loadAsset("no_op_es3.frag"))
    .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
    .feedbackVaryings( { "position", "positionOrg", "velocity", "color", "extra"} )
    .attribLocation( "iPosition", 0 )
    .attribLocation( "iPositionOrg", 1 )
    .attribLocation( "iVel", 2 )
    .attribLocation( "iColor", 3 )
    .attribLocation( "iExtra", 4 )
    );
    
    int FBO_SIZE = 2048;
    
    // frame buffer
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_SIZE, FBO_SIZE, fboFormatEnv.colorTexture() );
    
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;
    gl::draw( texture, Rectf( 0, 0, mFboEnv->getWidth(), mFboEnv->getHeight() ) );
    
    console() << "Proj * View Matrix : " << endl;
    console() << mtxProj << endl;
    
    // save color
    gl::ScopedGlslProg prog( mShaderInit );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );
    mShaderInit->uniform("uShadowMatrix", mtxProj);
    mShaderInit->uniform("uModelMatrix", mtxModel);
    mShaderInit->uniform("uTranslate", pos);
    gl::ScopedTextureBind texScope( texture, (uint8_t) 0 );
    mShaderInit->uniform( "uShadowMap", 0 );
    
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    std::swap( mSourceIndex, mDestinationIndex );
}

void ViewParticles::render() {
    gl::ScopedMatrices matScp;
    gl::setModelMatrix( mtxModel );
    gl::translate( pos );
    gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the
    
    
    // render particles
    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

