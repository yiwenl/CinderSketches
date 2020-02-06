#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "CinderARKit.h"
#include "BatchBall.hpp"
#include "BatchGridDots.hpp"
#include "BatchAxis.hpp"

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
    
    BatchBallRef        mBall;
    BatchGridDotsRef    mGrid;
    BatchAxisRef        mAxis;
};

void PixelatedApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Horizontal );
    
    mARSession.runConfiguration( config );
    
    
    mBall = BatchBall::create();
    mGrid = BatchGridDots::create();
    mAxis = BatchAxis::create();
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
    
    mAxis->draw();
    mGrid->draw(0.1f);
    mBall->draw(vec3(0.0, 0.0, -0.1), vec3(0.005f), vec3(1.0, 0.0, 0.0));
}

CINDER_APP( PixelatedApp, RendererGl )
