#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "CinderARKit.h"
#include "BatchHelpers.h"
#include "ViewParticles.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

const int    FBO_WIDTH  = 2048;
const int    FBO_HEIGHT = 2048;

class Pixelated02App : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    ARKit::Session          mARSession;
    BatchBallRef            bBall;
    
    vector<ViewParticlesRef>    particleViews;
    gl::FboRef                  mFboEnv;
};

void Pixelated02App::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Vertical );
    
    mARSession.runConfiguration( config );
    
    
    // helpers
    bBall = BatchBall::create();
    
    
    // frame buffer
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormatEnv.colorTexture() );
}

void Pixelated02App::touchesBegan( TouchEvent event )
{
    mARSession.addAnchorRelativeToCamera( vec3(0.0f, 0.0f, -0.5f) );
}

void Pixelated02App::update()
{
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;

    mARSession.drawRGBCaptureTexture(getWindowBounds());
}

void Pixelated02App::draw()
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
        
        
        if(particleViews.size() == 0) {
            ViewParticlesRef view = ViewParticles::create(a.mUid, a.mTransform, a.mCenter, mFboEnv->getColorTexture());
                                                            
            particleViews.push_back(view);
        }
        
    }
    
    gl::enableDepth();
    
    for(int i=0; i<particleViews.size(); i++) {
        ViewParticlesRef view = particleViews.at(i);
        view->render();
    }
    
    gl::disableDepthRead();
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int ss = 128 * 2;
    gl::draw( mFboEnv->getColorTexture(), Rectf( 0, 0, ss, ss/getWindowAspectRatio() ) );
    
    if(particleViews.size() > 0) {
        gl::draw( particleViews.at(0)->texture, Rectf( ss, 0, ss * 2, ss/getWindowAspectRatio() ) );
    }
}

CINDER_APP( Pixelated02App, RendererGl )
