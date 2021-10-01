#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "CinderARKit.h"
#include "cinder/ObjLoader.h"
#include "Utils.hpp"
#include "Helpers.h"
#include "EaseNumber.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class MidAutumnApp : public App {
  public:
	void setup() override;
    void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session mARSession;
    
private :
    TriMeshRef          mMesh;
    gl::BatchRef        mBatchMoon;
    gl::BatchRef        mBatchFloor;
    gl::GlslProgRef     mShaderMoon;
    
    gl::Texture2dRef    mTexColor;
    
    alfrid::EaseNumberRef   mOffsetScale;
    
    vec3 mHit;
    vec3 mPosition;
    bool hasTouched = false;
};

void MidAutumnApp::setup()
{
    gl::enableFaceCulling();
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Horizontal );
    
    mARSession.runConfiguration( config );
    
    // 3d model
    ObjLoader loader( loadAsset("moon.obj"));
    mMesh = TriMesh::create(loader);
    auto plane = gl::VboMesh::create( geom::Plane().size(vec2(2.5f)));
    
    
    // shaders
    mShaderMoon = gl::GlslProg::create(loadAsset("moon.vert"), loadAsset("moon.frag"));
    mShaderMoon->uniform("uColorMap", 0);
    
    gl::GlslProgRef shaderFloor = gl::GlslProg::create(loadAsset("floor.vert"), loadAsset("floor.frag"));
    
    // draws
    mBatchMoon = gl::Batch::create(*mMesh, mShaderMoon);
    mBatchFloor = gl::Batch::create(plane, shaderFloor);
    
    // textures
    mTexColor = gl::Texture2d::create(loadImage( loadAsset("color.jpg")));
    
    // offsets
    mOffsetScale = alfrid::EaseNumber::create(0.0f);
    mOffsetScale->easing = 0.015f;
}

void MidAutumnApp::touchesBegan( TouchEvent event )
{
    auto anchors = mARSession.getPlaneAnchors();
    
    if(anchors.size() == 0) return;
        
    auto anchor = anchors.at(0);
    vec3 hit;
    
    Ray rayCam = Utils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    bool hasHit = Utils::hitTest(rayCam, anchor, &hit);
    
    if( hasHit) {
        mHit = vec3(hit);
        mOffsetScale->setValue(1.0f);
        hasTouched = true;
    }
}

void MidAutumnApp::update()
{
    if(hasTouched) {
        return;
    }
        
    auto anchors = mARSession.getPlaneAnchors();
    
    if(anchors.size() == 0) return;
        
    auto anchor = anchors.at(0);
    vec3 hit;
    
    Ray rayCam = Utils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    bool hasHit = Utils::hitTest(rayCam, anchor, &hit);
    
    if(hasHit) {
        mHit = vec3(hit);
        mPosition = mHit + vec3(0.0, 1.2, 0.0);
    }
}

void MidAutumnApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::disableDepthWrite();
    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::enableDepth();
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    float s = 1.0 - mOffsetScale->getValue();
    alfrid::helpers::drawBall(mHit, vec3(0.01 * s), vec3(1.0, 0.0, 0.0));
    
    
    if(hasTouched) {
        mat4 mFloor;
        
        mFloor = glm::translate(mFloor, mHit);
        gl::setModelMatrix(mFloor);
        
        gl::enableAdditiveBlending();
        mBatchFloor->draw();
        gl::enableAlphaBlending();
        
        mat4 mMoon;
        gl::setModelMatrix(mMoon);
        
        gl::ScopedGlslProg shaderMoon(mShaderMoon);
        gl::ScopedTextureBind tColor(mTexColor, 0);
        mShaderMoon->uniform("uPosition", mPosition);
        mShaderMoon->uniform("uScale", mOffsetScale->getValue() * 1.5f);
        mBatchMoon->draw();
    }
    
}

CINDER_APP( MidAutumnApp, RendererGl )
