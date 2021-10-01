//
//  DrawSave.hpp
//  ParticleExport01
//
//  Created by Yi-Wen LIN on 30/09/2021.
//

#ifndef DrawSave_hpp
#define DrawSave_hpp

#include <stdio.h>

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
