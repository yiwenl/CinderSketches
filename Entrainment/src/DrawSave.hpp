//
//  DrawSave.hpp
//  Entrainment
//
//  Created by Yi-Wen LIN on 16/04/2020.
//

#ifndef DrawSave_hpp
#define DrawSave_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class DrawSave {
public:
    
    DrawSave() {
        
    }
    
    void draw(gl::FboRef);
};



#endif /* DrawSave_hpp */
