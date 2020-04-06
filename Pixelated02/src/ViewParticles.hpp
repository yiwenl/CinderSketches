//
//  ViewParticles.hpp
//  Pixelated02
//
//  Created by Yi-Wen LIN on 09/03/2020.
//

#ifndef ViewParticles_hpp
#define ViewParticles_hpp

#include "cinder/gl/gl.h"
#include "CinderARKit.h"
#include <stdio.h>

#endif /* ViewParticles_hpp */


using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_PARTICLES = 10e4;


typedef std::shared_ptr<class ViewParticles> ViewParticlesRef;

class ViewParticles {
public:
    vec3 pos;
    mat4 mtxModel;
    mat4 mtxProj;
    gl::Texture2dRef texture;
    ARKit::AnchorID id;
    gl::FboRef mFboEnv;
    
    
    
    ViewParticles(ARKit::AnchorID mId, mat4 mMtxModel, mat4 mMtxProj, vec3 mPos, gl::Texture2dRef mTexture) {
        id = mId;
        mtxModel = mMtxModel;
        mtxProj = mMtxProj;
        pos = mPos; 
        texture = mTexture;
        
        init();
    }
    
    static ViewParticlesRef create(
        ARKit::AnchorID mId,
        mat4 mMtxModel,
        mat4 mMtxProj,
        vec3 mPos,
        gl::Texture2dRef mTexture
        ) { return std::make_shared<ViewParticles>(mId, mMtxModel, mMtxProj, mPos, mTexture); }
    
    void init();
    void render();

private:
    // shader
    gl::GlslProgRef     mShaderRender;
    gl::GlslProgRef     mShaderInit;
    gl::GlslProgRef     mShaderUpdate;
    
    
    // particles
    gl::VaoRef          mAttributes[2];
    gl::VboRef          mParticleBuffer[2];
    
    std::uint32_t       mSourceIndex        = 0;
    std::uint32_t       mDestinationIndex   = 1;
};
