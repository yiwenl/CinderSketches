//
//  BatchAxis.hpp
//  Pixelated
//
//  Created by Yi-Wen LIN on 06/02/2020.
//

#ifndef BatchAxis_hpp
#define BatchAxis_hpp

#include "cinder/gl/gl.h"
#include <stdio.h>

#endif /* BatchAxis_hpp */

using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::shared_ptr<class BatchAxis> BatchAxisRef;


class BatchAxis {
public:
    BatchAxis() {
        
    }
    static BatchAxisRef create() { return std::make_shared<BatchAxis>(); }
    
    void draw() {
        gl::ScopedColor scp;
        
        gl::lineWidth(2.0f);
        gl::color( ColorA( 1, 0, 0, 1.0f ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( vec3(-1000.0, 0.0, 0.0));
        gl::vertex( vec3( 1000.0, 0.0, 0.0));
        gl::end();
        
        gl::color( ColorA( 0, 1, 0, 1.0f ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( vec3(0.0, -1000.0, 0.0));
        gl::vertex( vec3(0.0,  1000.0, 0.0));
        gl::end();
        
        gl::color( ColorA( 0, 0, 1, 1.0f ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( vec3(0.0, 0.0, -1000.0));
        gl::vertex( vec3(0.0, 0.0,  1000.0));
        gl::end();
    }
};
