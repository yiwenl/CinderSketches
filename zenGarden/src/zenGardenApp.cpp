#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"


#include "CinderARKit.h"
#include "BatchHelpers.h"
#include "Utils.hpp"

#include "ViewFlower.hpp"
#include "ViewBackground.hpp"
#include "ViewBall.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class zenGardenApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session mARSession;
    
    
private:
    void updateEnvMap();
    
    
    BatchBallRef bBall;
    vector<ViewFlowerRef>   _flowers;
    gl::FboRef              mFboEnv;
    gl::FboRef              mFboMap;
    ViewBackground*         _vBg;
    ViewBall*               _vBall;
};

void zenGardenApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Both );
    
    mARSession.runConfiguration( config );
    
    
    bBall = BatchBall::create();
    
    int FBO_SIZE = 2048;
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_SIZE, FBO_SIZE, fboFormatEnv.colorTexture() );
    
    gl::Fbo::Format fboFormatMap;
    mFboMap = gl::Fbo::create( FBO_SIZE, FBO_SIZE, fboFormatMap.colorTexture() );
    
    
    _vBg = new ViewBackground();
    _vBall = new ViewBall();
}

void zenGardenApp::touchesBegan( TouchEvent event )
{
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    
    for(auto a: mARSession.getPlaneAnchors()) {
        vec3 hit;
        
        
        bool hasHit = Utils::hitTest(rayCam, a, &hit);
        
        if(hasHit) {
    
            ViewFlowerRef vFlower = ViewFlower::create(hit);
            _flowers.push_back(vFlower);
            
            return;
        }
    }
}

void zenGardenApp::updateEnvMap() {
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );
    mARSession.drawRGBCaptureTexture(getWindowBounds());
    
    
    gl::ScopedFramebuffer fboMap( mFboMap );
    gl::ScopedViewport viewportMap( vec2( 0.0f ), mFboMap->getSize() );
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::enableAdditiveBlending();
    
        
    for(auto flower:_flowers) {
        float s = 0.08f * flower->getOpeningOffset();
        _vBall->draw(flower->getTop(), vec3(s), mARSession.getCameraPosition());
    }
    
    gl::enableDepth();
    gl::enableAlphaBlending();
}


void zenGardenApp::update()
{
    for(auto flower: _flowers) {
        flower->update();
    }
    
    _vBg->update();
}

void zenGardenApp::draw()
{
    updateEnvMap();
	gl::clear( Color( 0, 0, 0 ) );

    gl::disableDepthRead();
    gl::disableDepthWrite();
    
//    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
//    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    _vBg->render(mFboEnv->getColorTexture(), mFboMap->getColorTexture());
    
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    
    gl::enableDepth();
    
    for(auto a: mARSession.getPlaneAnchors()) {
        vec3 hit;
        
        
        bool hasHit = Utils::hitTest(rayCam, a, &hit);
        
        if(hasHit) {
            bBall->draw(hit, vec3(0.001f), vec3(0.8));
        }
    }
    
    
    for(auto flower:_flowers) {
        flower->render();
    }
    
}

CINDER_APP( zenGardenApp, RendererGl )
