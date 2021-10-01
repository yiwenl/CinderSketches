//
//  Draw.hpp
//  PostProcessing
//
//  Created by Yi-Wen LIN on 25/05/2020.
//

#ifndef Draw_hpp
#define Draw_hpp

#include <stdio.h>
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class Draw {
public :
    gl::GlslProgRef shader;
    
    Draw() {
        // do nothing
    }
    
    void draw() {
        mBatch->draw();
    }
    
    Draw* setMesh(gl::VboMeshRef mMesh) {
        _mesh = mMesh;
        return this;
    }
    
    Draw* useProgram(string vsPath, string fsPath) {
        _vsPath = vsPath;
        _fsPath = fsPath;
        return this;
    }
    
    Draw* bindTexture(string mName, gl::Texture2dRef tex, int index) {
        _checkInit();
        tex->bind(index);
        shader->uniform(mName, index);
        return this;
    }
    
    Draw* uniform(string mName, float value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, vec2 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, vec3 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, vec4 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, int value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, ivec2 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, ivec3 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, ivec4 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, mat2 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, mat3 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
    Draw* uniform(string mName, mat4 value) {
        _checkInit();
        shader->uniform(mName, value);
        return this;
    }
    
private :
    gl::BatchRef mBatch;
    bool _hasInit = false;
    string _vsPath;
    string _fsPath;
    gl::VboMeshRef _mesh;
    
    void _checkInit() {
        if(_hasInit) {
            return;
        }
        
        shader = gl::GlslProg::create( loadAsset( _vsPath ), loadAsset( _fsPath ) );
        auto plane = gl::VboMesh::create( geom::Plane().normal(vec3(0, 0, 1)) );
        mBatch = gl::Batch::create(_mesh, shader);
        
        _hasInit = true;
    }
};

#endif /* Draw_hpp */
