//
//  ViewBackground.cpp
//  zenGarden
//
//  Created by Yi-Wen LIN on 29/04/2020.
//

#include "ViewBackground.hpp"


void ViewBackground::_init() {
//    console() << "INIT view background" << endl;
    
    auto plane = gl::VboMesh::create( geom::Plane().normal(vec3(0, 0, 1)) );
    _mShader = gl::GlslProg::create( loadAsset( "bg.vert" ), loadAsset( "bg.frag" ) );
    
    _bDraw = gl::Batch::create(plane, _mShader);
    
}


void ViewBackground::update() {
    
    
}


void ViewBackground::render(gl::Texture2dRef texture) {
    gl::ScopedGlslProg prog( _mShader );
    
    gl::ScopedTextureBind texScopeEnv( texture, (uint8_t) 0 );
    _mShader->uniform( "uEnvMap", 0 );
    _mShader->uniform( "uTime", (float)getElapsedSeconds() );
    _mShader->uniform( "uRatio", getWindowAspectRatio());
        
    _bDraw->draw();
    
}
