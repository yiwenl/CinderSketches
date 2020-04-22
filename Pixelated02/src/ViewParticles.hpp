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
#include "EaseNumber.hpp"


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
    float mSeed;
    
    
    
    ViewParticles() {
        init();
    }
    
    static ViewParticlesRef create() { return std::make_shared<ViewParticles>(); }
    
    void reset(ARKit::AnchorID mId, mat4 mMtxModel, mat4 mMtxProj, vec3 mPos, gl::Texture2dRef mTexture);
    void render();
    void update();
    void open();
    void init();

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
    
    // offsets
    EaseNumberRef       _offset;
    
    bool                _hasInit = false;
};

#endif /* ViewParticles_hpp */
