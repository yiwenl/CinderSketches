#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Config.hpp"
#include "BatchHelpers.h"
#include "CinderARKit.h"
#include "FboPingPong.hpp"
#include "DrawSave.hpp"
#include "DrawParticles.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class EntrainmentApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session mARSession;
    
private:
    BatchBallRef bBall;
    
    // fbo
    FboPingPongRef        mFbo;
    DrawParticlesRef      mDrawParticles;
};

void EntrainmentApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Horizontal );
    
    mARSession.runConfiguration( config );
    
    // helpers
    bBall = BatchBall::create();
    
    // particles
    int size = Config::getInstance().NUM_PARTICLES;
    
   // init fbo
   auto texFormat = gl::Texture::Format().internalFormat( GL_RGBA16F ).dataType(GL_FLOAT).minFilter(GL_NEAREST).magFilter(GL_NEAREST);
   gl::Fbo::Format format1;
   format1.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) )
   .attachment( GL_COLOR_ATTACHMENT1, gl::Texture2d::create( size, size, texFormat ) )
   .attachment( GL_COLOR_ATTACHMENT2, gl::Texture2d::create( size, size, texFormat ) )
   .attachment( GL_COLOR_ATTACHMENT3, gl::Texture2d::create( size, size, texFormat ) );
    
   
   gl::Fbo::Format format2;
   format2.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( size, size, texFormat ) )
   .attachment( GL_COLOR_ATTACHMENT1, gl::Texture2d::create( size, size, texFormat ) )
   .attachment( GL_COLOR_ATTACHMENT2, gl::Texture2d::create( size, size, texFormat ) )
   .attachment( GL_COLOR_ATTACHMENT3, gl::Texture2d::create( size, size, texFormat ) );
   
   mFbo = FboPingPong::create(size, size, format1, format2);
   
   DrawSave* drawSave = new DrawSave();
   drawSave->draw(mFbo->read());
    
    // draw calls
    mDrawParticles = DrawParticles::create();
}

void EntrainmentApp::touchesBegan( TouchEvent event )
{
    mARSession.addAnchorRelativeToCamera( vec3(0.0f, 0.0f, -0.5f) );
}

void EntrainmentApp::update()
{
}

void EntrainmentApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    
    
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = Config::getInstance().NUM_PARTICLES * 4;
    gl::draw( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT0), Rectf( 0, 0, s, s ) );
    
    gl::draw( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT2), Rectf( s, 0, s * 2, s ) );
}

CINDER_APP( EntrainmentApp, RendererGl )
