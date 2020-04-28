#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"


#include "CinderARKit.h"
#include "BatchHelpers.h"
#include "Utils.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class zenGardenApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session mARSession;
    
    
private:
    BatchBallRef bBall;
};

void zenGardenApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Both );
    
    mARSession.runConfiguration( config );
    
    
    bBall = BatchBall::create();
}

void zenGardenApp::mouseDown( MouseEvent event )
{
}

void zenGardenApp::update()
{
}

void zenGardenApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
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
    
    
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    
    for(auto a: mARSession.getPlaneAnchors()) {
        vec3 hit;
        
        
        bool hasHit = Utils::hitTest(rayCam, a, &hit);
        
        if(hasHit) {
            bBall->draw(hit, vec3(0.0025f), vec3(1.0, 0.0, 0.0));
        }
    }
    
}

CINDER_APP( zenGardenApp, RendererGl )
