//
//  FboPingPong.hpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#ifndef FboPingPong_hpp
#define FboPingPong_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"



using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::shared_ptr<class FboPingPong> FboPingPongRef;

class FboPingPong {
public:
    int index = 0;
    
    vector<gl::FboRef> mFbos;
    
    
    FboPingPong(int mWidth, int mHeight, gl::Fbo::Format mFormat1, gl::Fbo::Format mFormat2) {
        gl::FboRef fbo0 = gl::Fbo::create(mWidth, mHeight, mFormat1);
        gl::FboRef fbo1 = gl::Fbo::create(mWidth, mHeight, mFormat2);
        
        mFbos.push_back(fbo0);
        mFbos.push_back(fbo1);
    }
    
    static FboPingPongRef create(int mWidth, int mHeight, gl::Fbo::Format mFormat1, gl::Fbo::Format mFormat2) {
        return std::make_shared<FboPingPong>(mWidth, mHeight, mFormat1, mFormat2);
    }
    
    void swap() {
        index = index == 0 ?
        1 : 0;
    }
    
    gl::FboRef read() {
        return mFbos[index];
    }
    
    gl::FboRef write() {
        return mFbos[1 - index];
    }
};
#endif /* FboPingPong_hpp */
