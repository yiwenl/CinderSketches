//
//  DrawRender.hpp
//  ParticleExport01
//
//  Created by Yi-Wen LIN on 30/09/2021.
//

#ifndef DrawRender_hpp
#define DrawRender_hpp

#include <stdio.h>

using namespace ci;
using namespace ci::app;
using namespace std;


typedef std::shared_ptr<class DrawRender> DrawRenderRef;

class DrawRender {
public:
    
    gl::VaoRef      mVao;
    gl::GlslProgRef mShaderRender;
    
    DrawRender(){
        _init();
    }
    
    void render(gl::FboRef);
    
    static DrawRenderRef create() { return std::make_shared<DrawRender>(); }
    
private:
    void _init();
};


#endif /* DrawRender_hpp */
