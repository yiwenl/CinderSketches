#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

#include "BatchHelpers.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RayCastingApp : public App {
  public:
	void setup() override;
	void touchesMoved( TouchEvent event ) override;
    void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    // camera
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    // helpers
    
    BatchAxisRef        bAxis;
    BatchGridDotsRef    bDots;
    BatchPlaneRef       bPlane;
    BatchBallRef        bBall;
    
    Ray                 ray;
    vec3                hit;
};

void prepareSettings( RayCastingApp::Settings *settings) {
    settings->setWindowSize(1280, 720);
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( true );
}

void RayCastingApp::setup()
{
    setFrameRate(60.0f);
    gl::enableDepth();
    gl::enableFaceCulling();
        
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 100.0f );
    mCam.lookAt( vec3( 3.0, 3.0, 8.0), vec3( 0.0f ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    bAxis = BatchAxis::create();
    bDots = BatchGridDots::create();
    bPlane = BatchPlane::create();
    bBall = BatchBall::create();
    
    
    
}

void RayCastingApp::mouseDown( MouseEvent event ) {
    console() << "Mouse down" << endl;
}

void RayCastingApp::touchesMoved( TouchEvent event )
{
   
    
}

void RayCastingApp::update()
{
    float r = 2.0f;
    float time = getElapsedSeconds();
    float x = cos(time) * r;
    float z = sin(time) * r;
    
    vec3 pos = vec3(x, 2.0, z);
    vec3 dir = vec3(0.0) - pos;
    r = 0.5;
    time *= 0.389745164;
    x = sin(time) * r;
    z = cos(time) * r;
    dir = vec3(x, -1.0, z);
    dir = normalize(dir);
    
    
    ray = Ray(pos, dir);
    float distance = 0.0f;
    
    bool hasHit = ray.calcPlaneIntersection(vec3(0.0), vec3(0.0, 1.0, 0.0), &distance);
    if(hasHit) {
        hit = pos + dir * distance;
    }
}

void RayCastingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCam);
    
    bAxis->draw();
    bDots->draw();
    
    int s = 4.0;
    
    bPlane->draw(vec3(0.0), vec3(s, 0.0, s), vec3(0.9), 0.5);
    
    
    bBall->draw(ray.getOrigin(), vec3(0.1), vec3(1.0, 1.0, 0.0));
    vec3 b = ray.getOrigin() + ray.getDirection() * 4.0f;
    bBall->draw(ray.getOrigin(), vec3(0.05), vec3(0.0, 1.0, 1.0));
    BatchLine::draw(ray.getOrigin(), b, vec3(1.0));
    
    bBall->draw(hit, vec3(0.1), vec3(1.0, 0.0, 0.0));
    
}

CINDER_APP( RayCastingApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
