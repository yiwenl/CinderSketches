#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderARKit.h"
#include "BatchHelpers.h"

using namespace ci;
using namespace ci::app;
using namespace std;


struct DetectedSurface {
    vec3 origin;
    vec3 normal;
};

class PlaneAnchorApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    ARKit::Session mARSession;
    
    vector<vec3> mHits;
    vector<DetectedSurface> mPlanes;
    
    
    BatchBallRef    bBall;
};

void PlaneAnchorApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Both );
    
    mARSession.runConfiguration( config );
    
    
    bBall = BatchBall::create();
}

void PlaneAnchorApp::mouseDown( MouseEvent event )
{
}

void PlaneAnchorApp::update()
{
}

void PlaneAnchorApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::disableDepthRead();
    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    gl::ScopedGlslProg glslProg( gl::getStockShader( gl::ShaderDef().color() ));
    gl::ScopedColor colScp;
    gl::color( 1.0f, 1.0f, 1.0f );
    
    gl::enableDepth();
    
    mPlanes.clear();
    
    for (const auto& a : mARSession.getPlaneAnchors())
    {
        gl::ScopedMatrices matScp;
        gl::setModelMatrix( a.mTransform );
        
        vec3 pos = vec3(a.mTransform * vec4(0.0, 0.0, 0.0, 1.0));
        
        
        vec4 _dir = vec4(0.0, 1.0, 0.0, 0.0);
        vec3 dir = vec3(a.mTransform * _dir);
        
        DetectedSurface plane;
        plane.origin = vec3(pos);
        plane.normal = vec3(dir);
        mPlanes.push_back(plane);
        
        
        gl::translate( a.mCenter );
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
        const float xRad = a.mExtent.x * 0.5f;
        const float zRad = a.mExtent.z * 0.5f;
        gl::color( 0.0f, 0.6f, 0.9f, 0.2f );
        gl::drawSolidRect( Rectf( -xRad,-zRad, xRad, zRad ));
    }
    
    float s = 0.01;
    vec3 target;
    for (auto plane: mPlanes) {
        bBall->draw(plane.origin, vec3(s), vec3(1, 0, 0));
        
        target = plane.origin + plane.normal * 0.2f;
        bBall->draw(target, vec3(s), vec3(1, 1, 0));
        
        BatchLine::draw(plane.origin, target, vec3(1));
    }
}

CINDER_APP( PlaneAnchorApp, RendererGl )
