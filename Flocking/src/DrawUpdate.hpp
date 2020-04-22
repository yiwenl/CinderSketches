//
//  DrawUpdate.hpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#ifndef DrawUpdate_hpp
#define DrawUpdate_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "FboPingPong.hpp"


using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::shared_ptr<class DrawUpdate> DrawUpdateRef;


class DrawUpdate {
    
public:
    gl::GlslProgRef mShader;
    gl::BatchRef    mBatch;
    gl::Texture2dRef mTexTest;
    
    // methods
    DrawUpdate() {
        _init();
    }
    
    void render(FboPingPongRef mFbo);
    static DrawUpdateRef create() { return std::make_shared<DrawUpdate>(); }
    
private:
    void _init();
};
#endif /* DrawUpdate_hpp */
