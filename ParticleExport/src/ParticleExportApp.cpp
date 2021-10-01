#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cmath"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/Fbo.h"
#include "cinder/GeomIo.h"
#include "cinder/Perlin.h"

#include "cinder/Log.h"

#include "BatchHelpers.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class ParticleExportApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    float mSeed;
    
    // cameras
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    alfrid::BatchAxisRef    bAxis;
};

void prepareSettings( ParticleExportApp::Settings *settings) {
    settings->setWindowSize(1280, 720);
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}

void ParticleExportApp::setup()
{
    setFrameRate(60.0f);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::enable( GL_POINT_SPRITE_ARB ); // or use: glEnable
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );   // or use: glEnable

    mSeed = randFloat(1000.0f);
    
    // setup cameras
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.5f, 500.0f );
    mCam.lookAt( vec3( 0.0, 0.0, 5.0), vec3( 0.0f ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    // helpers
    bAxis = alfrid::BatchAxis::create();
}

void ParticleExportApp::mouseDown( MouseEvent event )
{
}

void ParticleExportApp::update()
{
}

void ParticleExportApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCam);
    bAxis->draw();
}

//CINDER_APP( ParticleExportApp, RendererGl )
CINDER_APP( ParticleExportApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )

