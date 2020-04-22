//
//  BatchLine.hpp
//  RayCasting
//
//  Created by Yi-Wen LIN on 21/04/2020.
//

#ifndef BatchLine_hpp
#define BatchLine_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BatchLine {
public:
    
    BatchLine() {
        
    }
    
    static void draw(vec3 mPointA, vec3 mPointB) {
        draw(mPointA, mPointB, vec3(1.0), 1.0f);
    }
    
    static void draw(vec3 mPointA, vec3 mPointB, vec3 mColor) {
        draw(mPointA, mPointB, mColor, 1.0f);
    }
    
    static void draw(vec3 mPointA, vec3 mPointB, vec3 mColor, float mOpacity) {
        gl::ScopedColor scp;
        
        gl::lineWidth(2.0f);
        gl::color( ColorA( mColor.x, mColor.y, mColor.z, mOpacity ) );
        gl::begin( GL_LINE_STRIP );
        gl::vertex( mPointA );
        gl::vertex( mPointB );
        gl::end();
        
    }
    
};

#endif /* BatchLine_hpp */
