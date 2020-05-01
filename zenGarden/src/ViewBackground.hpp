//
//  ViewBackground.hpp
//  zenGarden
//
//  Created by Yi-Wen LIN on 29/04/2020.
//

#ifndef ViewBackground_hpp
#define ViewBackground_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class ViewBackground {
public:
    ViewBackground() {
        _init();
    }
    
    
    void update();
    void render(gl::Texture2dRef);
    
    
private:
    void _init();
    
    gl::GlslProgRef _mShader;
    gl::BatchRef _bDraw;
};

#endif /* ViewBackground_hpp */
