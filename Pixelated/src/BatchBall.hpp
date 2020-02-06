//
//  BatchBall.hpp
//  Pixelated
//
//  Created by Yi-Wen LIN on 06/02/2020.
//

#ifndef BatchBall_hpp
#define BatchBall_hpp


#include "cinder/gl/gl.h"
#include <stdio.h>

#endif /* BatchBall_hpp */

using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::shared_ptr<class BatchBall> BatchBallRef;

class BatchBall {
    
public:
    BatchBall();

    
    void draw(vec3 position);
    void draw(vec3 position, vec3 scale);
    void draw(vec3 position, vec3 scale, vec3 color);
    void draw(vec3 position, vec3 scale, vec3 color, float opacity);
    
    static BatchBallRef create() { return std::make_shared<BatchBall>(); }
    
protected:
    void _init();
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mShader;
};
