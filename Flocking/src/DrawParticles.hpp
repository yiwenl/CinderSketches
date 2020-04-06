//
//  DrawParticles.hpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#ifndef DrawParticles_hpp
#define DrawParticles_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#endif /* DrawParticles_hpp */

using namespace ci;
using namespace ci::app;
using namespace std;


typedef std::shared_ptr<class DrawParticles> DrawParticlesRef;

class DrawParticles {
public:
    
    gl::VaoRef      mVao;
    gl::GlslProgRef mShaderRender;
    
    DrawParticles(){
        _init();
    }
    
    void render(gl::FboRef);
    
    static DrawParticlesRef create() { return std::make_shared<DrawParticles>(); }
    
private:
    void _init();
};

