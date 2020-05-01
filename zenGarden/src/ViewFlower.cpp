//
//  ViewFlower.cpp
//  zenGarden
//
//  Created by Yi-Wen LIN on 28/04/2020.
//

#include "ViewFlower.hpp"


void ViewFlower::_init() {
    console() << "Init View Flower" << endl;
    
    timeStart = getElapsedSeconds();
    
    numLeaves = floor(randFloat(3, 6));
    numPetals = floor(randFloat(4, 7));
    
    _offset = EaseNumber::create(0);
    _offsetOpening = EaseNumber::create(0);
    
    _offset->easing = 0.02;
    _offsetOpening->easing = 0.015;
    
    _offset->setValue(1);
    
    // set top + controls
    _top = getPos(randFloat(12.0f, 18.0f), 1.0f);
    _ctrl0 = getPos(randFloat(0.3f, 0.4f) * _top.y, 3.0f);
    _ctrl1 = getPos(randFloat(0.6f, 0.7f) * _top.y, 3.0f);
    
    
    // stem
    int subDiv = 20;
    gl::VboMeshRef stem = gl::VboMesh::create( geom::Plane().subdivisions(vec2(4, subDiv)).normal(vec3(0, 0, 1)) );
    mShaderStem = gl::GlslProg::create( loadAsset( "stem.vert" ), loadAsset( "stem.frag" ) );
    _bStem = gl::Batch::create(stem, mShaderStem);
    
    
    // leaves
    int num = numLeaves;
    subDiv = 30;
    gl::VboMeshRef plane = gl::VboMesh::create( geom::Plane().subdivisions(vec2(1, subDiv)).normal(vec3(0, 0, 1)) );
    
        
    // instancing
    vector<InstanceData> instanceData;
    
    for(int i=0; i<num; i++) {
        
        float h = randFloat(15.0f, 10.0f);
        float r = 5.5f;
        
        InstanceData data;
        data.pos = getPos(0.0f, 0.5f);
        data.end = getPos(h, 1.5f);
        data.ctrl0 = getPos(h * randFloat(0.2, 0.3), r);
        data.ctrl1 = getPos(h * randFloat(0.6, 0.7), r);
        data.extra = vec3(randFloat(), randFloat(), randFloat());
        
        instanceData.push_back(data);
    }
    
    
    gl::VboRef mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, instanceData.size() * sizeof(InstanceData), instanceData.data(), GL_STATIC_DRAW );
    
    geom::BufferLayout instanceDataLayout;
    instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, sizeof( InstanceData ), offsetof( InstanceData, pos ), 1 /* per instance*/ );
    instanceDataLayout.append( geom::Attrib::CUSTOM_1, 3, sizeof( InstanceData ), offsetof( InstanceData, end ), 1 /* per instance*/ );
    instanceDataLayout.append( geom::Attrib::CUSTOM_2, 3, sizeof( InstanceData ), offsetof( InstanceData, ctrl0 ), 1 /* per instance*/ );
    instanceDataLayout.append( geom::Attrib::CUSTOM_3, 3, sizeof( InstanceData ), offsetof( InstanceData, ctrl1 ), 1 /* per instance*/ );
    instanceDataLayout.append( geom::Attrib::CUSTOM_4, 3, sizeof( InstanceData ), offsetof( InstanceData, extra ), 1 /* per instance*/ );
    
    plane->appendVbo( instanceDataLayout, mInstanceDataVbo );
    
    mShaderLeaves = gl::GlslProg::create( loadAsset( "leaves.vert" ), loadAsset( "leaves.frag" ) );
    
    _bLeaves = gl::Batch::create(plane, mShaderLeaves, {
        { geom::Attrib::CUSTOM_0, "aPosOffset" },
        { geom::Attrib::CUSTOM_1, "aEnd" },
        { geom::Attrib::CUSTOM_2, "aControl0" },
        { geom::Attrib::CUSTOM_3, "aControl1" },
        { geom::Attrib::CUSTOM_4, "aExtra" }
    } );
    
    
    // flower
    subDiv = 20;
    gl::VboMeshRef flower = gl::VboMesh::create( geom::Plane().subdivisions(vec2(subDiv)).normal(vec3(0, 0, 1)) );
    
    // instancing
    vector<vec3> extras;
    for(int i=0; i<numPetals; i++) {
        extras.push_back(vec3(i, randFloat(), randFloat()));
    }
    gl::VboRef mInstanceDataVboPetals = gl::Vbo::create( GL_ARRAY_BUFFER, extras.size() * sizeof(vec3), extras.data(), GL_STATIC_DRAW );
    geom::BufferLayout instanceDataLayoutPetals;
//    instanceDataLayoutPetals.append( geom::Attrib::CUSTOM_0, 3, sizeof( vec3 ), 0, 1 /* per instance*/ );
    instanceDataLayoutPetals.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance*/ );
    flower->appendVbo( instanceDataLayoutPetals, mInstanceDataVboPetals );
    
    mShaderFlower = gl::GlslProg::create( loadAsset( "flower.vert" ), loadAsset( "flower.frag" ) );
    
    _bFlower = gl::Batch::create(flower, mShaderFlower, {
        { geom::Attrib::CUSTOM_0, "aPosOffset" }
    } );
}


vec3 ViewFlower::getPos(float y, float r) {
    return vec3(randFloat(-r, r), y, randFloat(-r, r));
}


void ViewFlower::update() {
    _offset->update();
    _offsetOpening->update();
    
    if(getElapsedSeconds() - timeStart > 3.6 && _offsetOpening->getTargetValue() < 1.0f) {
        console() << " Flower Opening " << endl;
        _offsetOpening->setValue(1);
    }
}


void ViewFlower::render() {
    float time = getElapsedSeconds() * 0.5f;
    vec3 noise = perlin.dnoise(_top.x, _top.z, time);
    
    vec3 top = _top + noise;
    
//    console() << noise << endl;
    
    mat4 mtxModel;
    mtxModel = glm::translate(mtxModel, _pos);
    
    gl::ScopedMatrices matScp;
    gl::setModelMatrix(mtxModel);
    
    
    gl::ScopedGlslProg sLeaves( mShaderLeaves );
    mShaderLeaves->uniform("uOffset", _offset->getValue());
    mShaderLeaves->uniform("uTime", time);
    _bLeaves->drawInstanced(numLeaves);
    
    
    gl::ScopedGlslProg sStem( mShaderStem );
    mShaderStem->uniform("uOffset", _offset->getValue());
    mShaderStem->uniform("uControl0", _ctrl0);
    mShaderStem->uniform("uControl1", _ctrl1);
    mShaderStem->uniform("uEnd", top);
    _bStem->draw();
    
    
    gl::ScopedGlslProg sFlower( mShaderFlower );
    mShaderFlower->uniform("uOffset", _offsetOpening->getValue());
    mShaderFlower->uniform("uNum", (float)numPetals);
    mShaderFlower->uniform("uTime", time);
    mShaderFlower->uniform("uPos", top);
    _bFlower->drawInstanced(numPetals);
}
