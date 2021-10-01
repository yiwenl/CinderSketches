//
//  FluidSim.hpp
//  FluidSimulation
//
//  Created by Yi-Wen LIN on 30/09/2021.
//

#ifndef FluidSim_hpp
#define FluidSim_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"

#include "Draw.h"
#include "FboPingPong.hpp"


class FluidSim {
public :
    FluidSim();
    
    FluidSim(int mTextureSize);
    
    void update();
    void updateFlow(vec2 pos, vec2 dir, float strength, float radius, float noise);
    void updateFlow(vec2 pos, vec2 dir, float strength, float radius) {
        updateFlow(pos, dir, strength, radius, 0.0f);
    }
    void updateFlow(vec2 pos, vec2 dir, float strength) {
        updateFlow(pos, dir, strength, 1.0f, 0.0f);
    }
    void updateFlow(vec2 pos, vec2 dir) {
        updateFlow(pos, dir, 1.0f, 1.0f, 0.0f);
    }
    
    gl::Texture2dRef getVelocity();
    gl::Texture2dRef getDensity();
    gl::Texture2dRef getPressure();
    gl::Texture2dRef getDivergence();
    
    int TEXTURE_SIZE = 256;
    int TEXTURE_DOWNSAMPLE = 2;
    float DENSITY_DISSIPATION = 0.994;
    float VELOCITY_DISSIPATION = 0.996;
    float PRESSURE_DISSIPATION = 0.996;
    int PRESSURE_ITERATIONS = 20;
    float timestep = 0.0001f;
    
    
private :
    void initFbos();
    void initDrawCalls();
    void advect(gl::FboRef fbo, gl::Texture2dRef textureX, float dissipation);
    
    // Frame buffers
    FboPingPongRef fboVelocity;
    FboPingPongRef fboDensity;
    FboPingPongRef fboPressure;
    
    gl::FboRef fboDivergence;
    
    // Draw calls
    alfrid::Draw* drawAdvect;
    alfrid::Draw* drawDivergence;
    alfrid::Draw* drawClear;
    alfrid::Draw* drawJacobi;
    alfrid::Draw* drawGradient;
    alfrid::Draw* drawSplat;
    alfrid::Draw* drawAdd;
    
};

#endif /* FluidSim_hpp */
