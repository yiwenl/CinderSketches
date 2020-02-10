#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Camera.h"
#include "cinder/CameraUi.h"

#include "BatchHelpers.h"



using namespace ci;
using namespace ci::app;
using namespace std;

class FlockingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    // helpers
    BatchAxisRef            bAxis;
    BatchGridDotsRef        bDots;
};

void prepareSettings( FlockingApp::Settings *settings) {
//    settings->setWindowSize(1920, 1080);
    settings->setWindowSize(1280, 720);
//    settings->setWindowSize(1080 * 0.8, 1350 * 0.8);
    
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}


void FlockingApp::setup()
{
    setFrameRate(60.0f);
    gl::enableDepth();
    gl::enable( GL_POINT_SPRITE_ARB ); // or use: glEnable
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );   // or use: glEnable
    
    
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 50.0f );
    mCam.lookAt( vec3( -5.0, 5.0, 5.0), vec3( 0.0f ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    // helpers
    bAxis = BatchAxis::create();
    bDots = BatchGridDots::create();
}

void FlockingApp::mouseDown( MouseEvent event )
{
}

void FlockingApp::update()
{
}

void FlockingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedMatrices mcp;
    gl::setMatrices(mCam);
    
    bAxis->draw();
    bDots->draw();
}

CINDER_APP( FlockingApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
