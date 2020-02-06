#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "CinderARKit.h"
#include "BatchBall.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class PixelatedApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    
    ARKit::Session mARSession;
    
    BatchBallRef      mBall;
};

void PixelatedApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Horizontal );
    
    mARSession.runConfiguration( config );
    
    
    mBall = BatchBall::create();
}

void PixelatedApp::touchesBegan( TouchEvent event )
{
}

void PixelatedApp::update()
{
//    console() << "Camera Position : " << mARSession.getCameraPosition() << endl;
}

void PixelatedApp::draw()
{
    gl::disableDepthRead();
    gl::disableDepthWrite();
	gl::clear( Color( 0, 0, 0 ) );
    
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    mBall->draw(vec3(0.0), vec3(0.005f), vec3(1.0, 0.0, 0.0));
}

CINDER_APP( PixelatedApp, RendererGl )