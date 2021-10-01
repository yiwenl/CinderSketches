//
//  FluidSim.cpp
//  FluidSimulation
//
//  Created by Yi-Wen LIN on 30/09/2021.
//

#include "FluidSim.hpp"
#include "cinder/Log.h"

FluidSim::FluidSim() {
    CI_LOG_D("init fluid simulation");
    
    // frame buffers
    initFbos();
        
    // draw calls
    initDrawCalls();
}

FluidSim::FluidSim(int mTextureSize) {
    TEXTURE_SIZE = mTextureSize;
    
    // frame buffers
    initFbos();
        
    // draw calls
    initDrawCalls();
}


void FluidSim::initDrawCalls() {
    auto plane = gl::VboMesh::create( geom::Plane() );
    vec2 texelSize = vec2(1.0) / vec2(float(TEXTURE_SIZE));

    drawAdvect = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "advect.frag" )->uniform("uTexelSize", texelSize)->uniform("uTimestep", timestep);
    drawDivergence = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "divergence.frag" )->uniform("uTexelSize", texelSize);
    drawClear = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "clear.frag" )->uniform("uDissipation", PRESSURE_DISSIPATION);
    drawJacobi = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "jacobi.frag" )->uniform("uTexelSize", texelSize);
    drawGradient = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "gradientSubstract.frag" )->uniform("uTexelSize", texelSize);
    drawAdd = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "add.frag" );
    drawSplat = (new alfrid::Draw())->setMesh(plane)->useProgram( "pass.vert" , "splat.frag" );
    
}

void FluidSim::initFbos() {
    int size = TEXTURE_SIZE;
    auto texFormat = gl::Texture::Format().internalFormat( GL_RGBA32F ).dataType(GL_FLOAT).minFilter(GL_LINEAR).magFilter(GL_LINEAR).wrap(GL_CLAMP_TO_EDGE);
    
    // velocity
    gl::Fbo::Format formatVel1;
    formatVel1.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    gl::Fbo::Format formatVel2;
    formatVel2.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    
    fboVelocity = FboPingPong::create(size, size, formatVel1, formatVel2);
    
    // velocity
    gl::Fbo::Format formatDen1;
    formatDen1.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    gl::Fbo::Format formatDen2;
    formatDen2.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    
    fboDensity = FboPingPong::create(size, size, formatDen1, formatDen2);
    
    // pressure
    gl::Fbo::Format formatPress1;
    formatPress1.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    gl::Fbo::Format formatPress2;
    formatPress2.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    
    fboPressure = FboPingPong::create(size, size, formatPress1, formatPress2);
    
    // divergence
    gl::Fbo::Format formatDivergence;
    formatDivergence.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) );
    
    fboDivergence = gl::Fbo::create(size, size, formatDivergence);
}

void FluidSim::updateFlow(vec2 pos, vec2 dir, float strength, float radius, float noise) {
    float _radius = 0.05 * radius;
    float _strength = strength * 500.0f;
    float time = app::getElapsedSeconds();
    
    // update velocity
    gl::ScopedFramebuffer fboVel(fboVelocity->write());
    gl::ScopedViewport viewportVel( vec2( 0.0f ), fboVelocity->write()->getSize() );
    gl::clear( Color(0, 0, 0));
    
    drawSplat
        ->uniform("uTime", time)
        ->uniform("uCenter", pos)
        ->uniform("uRadius", _radius)
        ->uniform("uStrength", _strength)
        ->uniform("uNoiseStrength", noise)
        ->uniform("uIsVelocity", 1.0f)
        ->uniform("uDir", dir)
        ->bindTexture("textureBase", getVelocity(), 0)
    ->draw();
    fboVelocity->swap();
    
    // update density
    gl::ScopedFramebuffer fboDen(fboDensity->write());
    gl::ScopedViewport viewportDen( vec2( 0.0f ), fboDensity->write()->getSize() );
    gl::clear( Color(0, 0, 0));
    
    drawSplat
        ->uniform("uTime", time)
        ->uniform("uCenter", pos)
        ->uniform("uRadius", _radius)
        ->uniform("uStrength", 0.05f)
        ->uniform("uNoiseStrength", noise)
        ->uniform("uIsVelocity", 0.0f)
        ->uniform("uDir", dir)
        ->bindTexture("textureBase", getDensity(), 0)
    ->draw();
    fboDensity->swap();
}

void FluidSim::advect(gl::FboRef fbo, gl::Texture2dRef textureX, float dissipation) {
    gl::ScopedFramebuffer fboAdvect(fbo);
    gl::ScopedViewport viewport( vec2( 0.0f ), fbo->getSize() );
    gl::clear( Color(0, 0, 0));
    
    drawAdvect
        ->bindTexture("textureVel", getVelocity(), 0)
        ->bindTexture("textureMap", textureX, 1)
        ->uniform("uDissipation", dissipation)
        ->uniform("uTimestep", timestep)
    ->draw();
}

void FluidSim::update() {
    // velocity
    advect(fboVelocity->write(), getVelocity(), VELOCITY_DISSIPATION);
    fboVelocity->swap();
    
    // density
    advect(fboDensity->write(), getDensity(), DENSITY_DISSIPATION);
    fboDensity->swap();
        
    // divergence
    gl::ScopedFramebuffer fboDiv(fboDivergence);
    gl::ScopedViewport vFboDiv( vec2( 0.0f ), fboDivergence->getSize() );
    gl::clear( Color(0, 0, 0));
    drawDivergence->bindTexture("textureVel", getVelocity(), 0)->draw();
    
    // clear
    gl::ScopedFramebuffer fboClear(fboPressure->write());
    gl::ScopedViewport vFboClear( vec2( 0.0f ), fboPressure->write()->getSize() );
    gl::clear( Color(0, 0, 0));
    drawClear->bindTexture("texturePressure", getPressure(), 0)->draw();
    fboPressure->swap();
    
    for(int i=0; i<PRESSURE_ITERATIONS; i++) {
        gl::ScopedFramebuffer _fbo(fboPressure->write());
        gl::ScopedViewport _viewport( vec2( 0.0f ), fboPressure->write()->getSize() );
        gl::clear( Color(0, 0, 0));
        
        drawJacobi->bindTexture("texturePressure", getPressure(), 0)->bindTexture("textureDivergence", getDivergence(), 1)->draw();
        fboPressure->swap();
    }
    
    // gradient sub
    gl::ScopedFramebuffer fboGradient(fboVelocity->write());
    gl::ScopedViewport vFboGradient( vec2( 0.0f ), fboVelocity->write()->getSize() );
    gl::clear( Color(0, 0, 0));
    
    drawGradient->bindTexture("texturePressure", getPressure(), 0)->bindTexture("textureVel", getVelocity(), 1)->draw();
    fboVelocity->swap();
}

gl::Texture2dRef FluidSim::getVelocity() {
    return fboVelocity->read()->getTexture2d(GL_COLOR_ATTACHMENT0);
}

gl::Texture2dRef FluidSim::getDensity() {
    return fboDensity->read()->getTexture2d(GL_COLOR_ATTACHMENT0);
}

gl::Texture2dRef FluidSim::getPressure() {
    return fboPressure->read()->getTexture2d(GL_COLOR_ATTACHMENT0);
}

gl::Texture2dRef FluidSim::getDivergence() {
    return fboDivergence->getTexture2d(GL_COLOR_ATTACHMENT0);
}
