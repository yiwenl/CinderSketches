#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

#include "BatchHelpers.h"
#include "FboPingPong.hpp"
#include "DrawSave.hpp"
#include "DrawParticles.hpp"



using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_PARTICLES = 64;


struct Particle
{
    vec3    pos;
    vec3    vel;
    vec3    random;
};


class FlockingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
    void                    initParticles();
    
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    // helpers
    BatchAxisRef            bAxis;
    BatchGridDotsRef        bDots;
    
    
    // fbo
    FboPingPongRef        mFbo;
    
    // drawcalls
    DrawParticlesRef      mDrawParticles;
    
    
    float mSeed = randFloat(10000.0f);
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
    
    initParticles();
    
    mDrawParticles = DrawParticles::create();
}

void FlockingApp::initParticles()
{
    
    // init fbo
    gl::Fbo::Format format;
    format.setColorTextureFormat( gl::Texture::Format().internalFormat( GL_RGBA32F ).dataType(GL_FLOAT).minFilter(GL_NEAREST).magFilter(GL_NEAREST) );
    
    mFbo = FboPingPong::create(NUM_PARTICLES, NUM_PARTICLES, format);
    
    DrawSave* drawSave = new DrawSave();
    drawSave->draw(mFbo->read());
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
    
    mDrawParticles->render(mFbo->read());
    
    
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = 128;
    gl::draw( mFbo->read()->getColorTexture(), Rectf( 0, 0, s, s ) );
    
}

CINDER_APP( FlockingApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
