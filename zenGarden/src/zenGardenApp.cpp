#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"


#include "CinderARKit.h"
#include "BatchHelpers.h"
#include "Utils.hpp"

#include "ViewFlower.hpp"
#include "ViewBackground.hpp"

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
    ViewBackground*         _vBg;
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
    
    
    _vBg = new ViewBackground();
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
    
    gl::ScopedMatrices matScp;
    
    mARSession.drawRGBCaptureTexture(getWindowBounds());
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
    
    _vBg->render(mFboEnv->getColorTexture());
    /*
    gl::ScopedGlslProg glslProg( gl::getStockShader( gl::ShaderDef().color() ));
    gl::ScopedColor colScp;
    gl::color( 1.0f, 1.0f, 1.0f );
    
    
    for (const auto& a : mARSession.getPlaneAnchors())
    {
        gl::ScopedMatrices matScp;
        gl::setModelMatrix( a.mTransform );
        gl::translate( a.mCenter );
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
        const float xRad = a.mExtent.x * 0.5f;
        const float zRad = a.mExtent.z * 0.5f;
        gl::color( 0.0f, 0.6f, 0.9f, 0.2f );
        gl::drawSolidRect( Rectf( -xRad,-zRad, xRad, zRad ));
    }
*/
    
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    
    gl::enableDepth();
    
    for(auto a: mARSession.getPlaneAnchors()) {
        vec3 hit;
        
        
        bool hasHit = Utils::hitTest(rayCam, a, &hit);
        
        if(hasHit) {
            bBall->draw(hit, vec3(0.001f), vec3(0.0));
        }
    }
    
    
    for(auto flower:_flowers) {
        flower->render();
    }
    
}

CINDER_APP( zenGardenApp, RendererGl )
