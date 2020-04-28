//
//  ViewFlower.cpp
//  zenGarden
//
//  Created by Yi-Wen LIN on 28/04/2020.
//

#include "ViewFlower.hpp"


void ViewFlower::_init() {
    console() << "Init View Flower" << endl;
    
    _offset = EaseNumber::create(0);
    _offset->easing = 0.015;
    _offset->setValue(1);
    
    // leaves
    int num = 5;
    int subDiv = 100;
    gl::VboMeshRef plane = gl::VboMesh::create( geom::Plane().subdivisions(vec2(1, subDiv)).normal(vec3(0, 0, 1)) );
        
    // instancing
    vector<InstanceData> instanceData;
    
    for(int i=0; i<num; i++) {
        
        float h = randFloat(15.0f, 12.0f);
        float r = 2.5f;
        
        InstanceData data;
        data.pos = getPos(0.0f, 2.0f);
        data.end = getPos(h, r);
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
}


vec3 ViewFlower::getPos(float y, float r) {
    return vec3(randFloat(-r, r), y, randFloat(-r, r));
}


void ViewFlower::update() {
    _offset->update();
}


void ViewFlower::render() {
    mat4 mtxModel;
    mtxModel = glm::translate(mtxModel, _pos);
    
    gl::ScopedMatrices matScp;
    gl::setModelMatrix(mtxModel);
    
    
    gl::ScopedGlslProg sLeaves( mShaderLeaves );
    mShaderLeaves->uniform("uOffset", _offset->getValue());
    _bLeaves->drawInstanced(5);
}
