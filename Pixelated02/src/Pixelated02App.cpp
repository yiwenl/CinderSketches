#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Rand.h"

#include "CinderARKit.h"
#include "BatchHelpers.h"
#include "ViewParticles.hpp"
#include "Utils.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

const int    FBO_WIDTH  = 2048;
const int    FBO_HEIGHT = 2048;
const int    NUM_VIEWS = 5;


class Pixelated02App : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    ARKit::Session          mARSession;
    BatchBallRef            bBall;
    BatchPlaneRef           bPlane;
    
    vector<ViewParticlesRef>    particleViews;
    gl::FboRef                  mFboEnv;
    
    
    int  mIndex = 0;
    
    vector<vec3> mHits;
    
    vec3 mHit;
    bool hasTouched = false;
    Ray rayTouch;
};

void Pixelated02App::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Horizontal );
    
    mARSession.runConfiguration( config );
    
    
    // helpers
    bBall = BatchBall::create();
    bPlane = BatchPlane::create();
    
    // frame buffer
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormatEnv.colorTexture() );
    
    
    // views
    
    for(int i=0; i<NUM_VIEWS; i++) {
        ViewParticlesRef view = ViewParticles::create();
        particleViews.push_back(view);
    }
    
    mHit = vec3(999.0);
}

void Pixelated02App::touchesBegan( TouchEvent event )
{
    
    auto anchors = mARSession.getPlaneAnchors();
    
    if(anchors.size() == 0) {
        return;
    }
    
    auto anchor = anchors.at(0);
    vec3 hit;
    
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    
    bool hasHit = Utils::hitTest(rayCam, anchor, &hit);
            
    if(hasHit) {
        mHits.push_back(hit);
        
//        if(mIndex == 0) {
            mat4 mtxProj = mARSession.getProjectionMatrix() * mARSession.getViewMatrix();
            
            ViewParticlesRef view = particleViews.at(mIndex);
            view->reset(anchor.mUid, anchor.mTransform, mtxProj, hit, mFboEnv->getColorTexture());
            
            view->open();
//        }
        
        mIndex++;
        
        if(mIndex == particleViews.size() ) {
            mIndex = 0;
        }
    }
}


void Pixelated02App::update()
{
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;

    mARSession.drawRGBCaptureTexture(getWindowBounds());
    
    
    for(const auto& view : particleViews) {
        view->update();
    }
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

    
    gl::enableDepth();
    
    
    auto anchors = mARSession.getPlaneAnchors();
    if(anchors.size() > 0) {
        auto anchor = anchors.at(0);
        vec3 hit;
        
        Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
        
        bool hasHit = Utils::hitTest(rayCam, anchor, &hit);
        
        if(hasHit) {
            bBall->draw(hit, vec3(0.0025f), vec3(1.0, 0.0, 0.0));
        }
    }
    
//
//
//    for(auto& mHit: mHits) {
//        bBall->draw(mHit, vec3(0.01f), vec3(1.0, 0.0, 1.0));
//    }
  
    for(int i=0; i<particleViews.size(); i++) {
        ViewParticlesRef view = particleViews.at(i);
        view->render();
    }
    
    /*
    gl::disableDepthRead();
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int ss = 128 * 2;
    gl::draw( mFboEnv->getColorTexture(), Rectf( 0, 0, ss, ss/getWindowAspectRatio() ) );
    
    if(particleViews.size() > 0) {
        gl::draw( particleViews.at(0)->texture, Rectf( ss, 0, ss * 2, ss/getWindowAspectRatio() ) );
    }
     
     
     */
}

CINDER_APP( Pixelated02App, RendererGl )
