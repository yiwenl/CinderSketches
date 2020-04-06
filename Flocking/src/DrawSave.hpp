//
//  DrawSave.hpp
//  Flocking
//
//  Created by Yi-Wen LIN on 06/04/2020.
//

#ifndef DrawSave_hpp
#define DrawSave_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#endif /* DrawSave_hpp */


using namespace ci;
using namespace ci::app;
using namespace std;


class DrawSave {
public:
    
    DrawSave() {
        
    }
    
    void draw(gl::FboRef);
};

