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
    // buffers
    
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );
    float range = 0.1;
    float range_z = 0.001;
    
    for( int i =0; i<particles.size(); i++) {
        float a = randFloat(M_PI * 2.0);
        float r = sqrt(randFloat()) * range;
        float x = cos(a) * r;
        float z = sin(a) * r;
        float y = randFloat(-range_z, range_z);
        
        auto &p = particles.at( i );
        
        p.pos = vec3(x, y, z);
        p.posOrg = vec3(x, y, z);
        p.vel = vec3(0, 0, 0);
        p.color = randVec3();
        p.extra = vec3(randFloat(), randFloat(), 0.0);
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
    
    
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) ).fragment( loadAsset("no_op_es3.frag"))
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
    
    // offset
    _offset = EaseNumber::create(0);
    _offset->easing = 0.025;
    
    
    // shadow mapping
    int fboSize = 1024;
    
    gl::Texture2d::Format depthFormat;
    depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
    depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
    depthFormat.setMagFilter( GL_LINEAR );
    depthFormat.setMinFilter( GL_LINEAR );
    depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    depthFormat.setCompareFunc( GL_LEQUAL );
    mShadowMapTex = gl::Texture2d::create( fboSize, fboSize, depthFormat );
    
    gl::Fbo::Format fboFormat;
    fboFormat.attachment( GL_DEPTH_ATTACHMENT, mShadowMapTex );
    _mFboShadow = gl::Fbo::create( fboSize, fboSize, fboFormat );
    
    mCamLight.setPerspective( 75.0f, _mFboShadow->getAspectRatio(), 0.3f, 3.0f );
    
    // floor
    
    mShaderShadow = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "floor.vert" ) ).fragment( loadAsset("floor.frag"))
    );
    
    auto plane = gl::VboMesh::create( geom::Plane() );
    mBatchFloor = gl::Batch::create(plane, mShaderShadow);
    
}

void ViewParticles::reset(ARKit::AnchorID mId, mat4 mMtxModel, mat4 mMtxProj, vec3 mPos, gl::Texture2dRef mTexture) {
    // console() << " Init :" << pos << ", " << NUM_PARTICLES << endl;
    
    mSeed = randFloat(-1.0f, 1.0f);
    
    id = mId;
    mtxModel = mat4(mMtxModel);
    mat4 modelInvert = mat4(mMtxModel);
    modelInvert = glm::inverse(modelInvert);
    pos = vec3(modelInvert * vec4(mPos, 1.0));
    mtxProj = mMtxProj;
    texture = mTexture;
    
    
    
    
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;
    gl::draw( texture, Rectf( 0, 0, mFboEnv->getWidth(), mFboEnv->getHeight() ) );
    
    
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
    
    
    // setup light camera
    vec3 mLightPos = pos + vec3(0.0, 1.5, -0.01);
    mCamLight.lookAt( mLightPos, pos);
    _mtxShadow = mCamLight.getProjectionMatrix() * mCamLight.getViewMatrix();
    
//    console() << "Reset : " << mLightPos << " -> " << pos << endl;
    
    _hasInit = true;
}


void ViewParticles::open() {
    _offset->setTo(1.0f);
    _offset->setValue(0.0f);
}


void ViewParticles::update() {
    // update offset value
    _offset->update();
    
    if(!_hasInit) {
        return;
    }
    _updateShadowMap();
    
    
    gl::ScopedGlslProg prog( mShaderUpdate );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage
    mShaderUpdate->uniform("uTime", float(getElapsedSeconds()) + mSeed);
    mShaderUpdate->uniform("uOffset", _offset->getValue());
    mShaderUpdate->uniform("uSeed", mSeed);
    
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    std::swap( mSourceIndex, mDestinationIndex );
}


void ViewParticles::_updateShadowMap() {
    
//    console() << "Update background : " << _mFboShadow->getSize() << endl;
    gl::ScopedFramebuffer fbo( _mFboShadow );
    gl::ScopedViewport viewport( vec2( 0.0f ), _mFboShadow->getSize() );
    gl::ScopedMatrices matScp;
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices( mCamLight );

    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));

    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}


void ViewParticles::render() {
    if(!_hasInit) { return; }
    gl::ScopedMatrices matScp;
    gl::setModelMatrix(mtxModel);
    
    // render particles
    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    
    gl::ScopedTextureBind texScope( mShadowMapTex, (uint8_t) 0 );
    mShaderRender->uniform( "uShadowMap", 0 );
    
    mShaderRender->uniform("uShadowMatrix", _mtxShadow);
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

void ViewParticles::renderFloor() {
    if(!_hasInit) { return; }
//    gl::ScopedMatrices matScp;
    
    // render particles
    gl::ScopedGlslProg prog( mShaderShadow );
    mShaderShadow->uniform("uShadowMatrix", _mtxShadow);
    mShaderShadow->uniform("uPosition", pos);
    
    gl::ScopedTextureBind texScope( mShadowMapTex, (uint8_t) 0 );
    mShaderShadow->uniform( "uShadowMap", 0 );
    
    mBatchFloor->draw();
}
