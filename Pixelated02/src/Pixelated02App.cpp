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
const int    NUM_VIEWS = 10;


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
    
    
    void resetView();
    void updateBackground();
};

void Pixelated02App::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Both );
    
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

void Pixelated02App::touchesBegan( TouchEvent event ) {
    resetView();
}


void Pixelated02App::resetView() {
    auto anchors = mARSession.getPlaneAnchors();
    
    if(anchors.size() == 0) return;
        
    auto anchor = anchors.at(0);
    vec3 hit;
    
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());   
    bool hasHit = Utils::hitTest(rayCam, anchor, &hit);
            
    if(hasHit) {
        mHits.push_back(hit);
        
        mat4 mtxProj = mARSession.getProjectionMatrix() * mARSession.getViewMatrix();
        
        ViewParticlesRef view = particleViews.at(mIndex);
        view->reset(anchor.mUid, anchor.mTransform, mtxProj, hit, mFboEnv->getColorTexture());
        
        view->open();

        
        mIndex++;
        
        if(mIndex == particleViews.size() ) {
            mIndex = 0;
        }
    }
}


void Pixelated02App::update() {
    updateBackground();
    
    
    gl::enableDepth();
    // update views
    for(const auto& view : particleViews) {
        view->update();
    }
}


void Pixelated02App::updateBackground() {
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;

    mARSession.drawRGBCaptureTexture(getWindowBounds());
}


void Pixelated02App::draw()
{
    // clear the context
	gl::clear( Color( 0, 0, 0 ) );
    
    
    // disable depth and draw background
    gl::disableDepthRead();
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    
    // set camera matrices from AR
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    // draw particle shadow
//    for(int i=0; i<particleViews.size(); i++) {
//        ViewParticlesRef view = particleViews.at(i);
//        view->renderFloor();
//    }
    
    gl::enableDepth();

    
    // draw hit point from camera to plane ( anchor )
    auto anchors = mARSession.getPlaneAnchors();

    /*
    float s = 0.01f;
    for(auto a: anchors) {
        vec3 pos = vec3(a.mTransform * vec4(0.0, 0.0, 0.0, 1.0));
        vec3 dir = vec3(a.mTransform * vec4(0.0, 1.0, 0.0, 0.0));
        vec3 target = pos + dir * 0.15f;
        
        bBall->draw(pos, vec3(s), vec3(1, 1, 0));
        bBall->draw(target, vec3(s), vec3(1, 1, 0));
        BatchLine::draw(pos, target, vec3(1, 1, 1));
    }
     */

    for(auto a: anchors) {
            vec3 hit;
            
            Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
            
            bool hasHit = Utils::hitTest(rayCam, a, &hit);
            
            if(hasHit) {
                bBall->draw(hit, vec3(0.0025f), vec3(1.0, 0.0, 0.0));
            }
    }
    
    
    
    
    /*
    vec3 dir = rayCam.getDirection();
    dir.y -= 0.2;
    rayCam.setDirection(dir);
        
    hasHit = Utils::hitTest(rayCam, anchor, &hit);
    
    if(hasHit) {
        bBall->draw(hit, vec3(0.005f), vec3(1.0, 1.0, 0.0));
    }
     
     */

    
    
  
    
    // draw particle views
    for(int i=0; i<particleViews.size(); i++) {
        ViewParticlesRef view = particleViews.at(i);
        view->render();
    }
    
    /*
     
    gl::disableDepthRead();
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int ss = 128 * 2;
    gl::draw( particleViews.at(0)->_mFboShadow->getColorTexture(), Rectf( 0, 0, ss, ss ) );
    gl::draw( particleViews.at(0)->mShadowMapTex, Rectf( ss, 0, ss * 2, ss ) );
    
    gl::draw( mFboEnv->getColorTexture(), Rectf( 0, 0, ss, ss/getWindowAspectRatio() ) );
    
    if(particleViews.size() > 0) {
        gl::draw( particleViews.at(0)->texture, Rectf( ss, 0, ss * 2, ss/getWindowAspectRatio() ) );
    }
     */
}

CINDER_APP( Pixelated02App, RendererGl )
