#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Camera.h"
#include "cinder/CameraUi.h"

#include "BatchHelpers.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class Particles002App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    
private:
    CameraPersp             mCam;
    CameraPersp             mCamLight;
    CameraUi                mCamUi;
    
    
    // helpers
    BatchAxisRef            bAxis;
    BatchGridDotsRef        bDots;
};

const int NUM_PARTICLES = 400e3;

void prepareSettings( Particles002App::Settings *settings) {
//    settings->setWindowSize(1920, 1080);
    settings->setWindowSize(1280, 720);
//    settings->setWindowSize(1080 * 0.8, 1350 * 0.8);
    
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}


void Particles002App::setup()
{
    setFrameRate(60.0f);
    gl::enableDepth();
    gl::enable( GL_POINT_SPRITE_ARB ); // or use: glEnable
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );   // or use: glEnable
    
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 5.0f );
    mCam.lookAt( vec3( 0.0, 0.0, 1.0), vec3( 0.0f ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    // helpers
    bAxis = BatchAxis::create();
    bDots = BatchGridDots::create();
}

void Particles002App::mouseDown( MouseEvent event )
{
}

void Particles002App::update()
{
}

void Particles002App::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::ScopedMatrices mcp;
    gl::setMatrices( mCam );
    
    bAxis->draw();
    bDots->draw(0.1);
}

CINDER_APP( Particles002App, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
