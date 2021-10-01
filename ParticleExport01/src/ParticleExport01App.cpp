#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"
#include "Helpers.h"
#include "OrbitalControl.h"

#include "Config.hpp"
#include "FboPingPong.hpp"
#include "DrawSave.hpp"
#include "DrawRender.hpp"
#include "DrawUpdate.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class ParticleExport01App : public App {
public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	void resize() override;

private:
	// camera
	CameraPersp			mCamera;
	OrbitalControl*     mOrbControl;

	// controls
    params::InterfaceGlRef mParams;
    float fps = 0.0f;
    
    void initParticles();
    
    // fbo
    FboPingPongRef        mFbo;
    
    // drawcalls
    DrawRenderRef           mDrawRender;
    DrawUpdateRef         mDrawUpdate;
    
    int frameCount = 0;
};


void prepareSettings( ParticleExport01App::Settings *settings) {
    settings->setHighDensityDisplayEnabled();
    settings->setMultiTouchEnabled( false );
    settings->setTitle("ParticleExport01App");
}


void ParticleExport01App::setup()
{
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setFrameRate(30.0f);
    
    gl::enableDepth();
    gl::enable( GL_POINT_SPRITE_ARB ); // or use: glEnable
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );   // or use: glEnable
    

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 50.0f);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
	mOrbControl = new OrbitalControl(&mCamera, getWindow());
	mOrbControl->rx->setValue(0.5);
    mOrbControl->ry->setValue(0.5);

	// controls
    mParams = params::InterfaceGl::create("Controls", vec2(200, 200));
    mParams->addParam("FPS", &fps);
    
    initParticles();
    
    mDrawRender = DrawRender::create();
    mDrawUpdate = DrawUpdate::create();
}

void ParticleExport01App::initParticles() {
    int size = Config::getInstance().NUM_PARTICLES;
    CI_LOG_D("Number of Particles :: " << size);
    
    auto texFormat = gl::Texture::Format().internalFormat( GL_RGBA16F ).dataType(GL_HALF_FLOAT).minFilter(GL_NEAREST).magFilter(GL_NEAREST);
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
}

void ParticleExport01App::keyDown( KeyEvent event )
{
	if (event.getCode() == 27) {
		quit();
	}

	if (event.getChar() == 'f') {
		setFullScreen(!isFullScreen());
	}
	else if (event.getChar() == 's') {
		alfrid::helpers::saveImage();
	}
}

void ParticleExport01App::update()
{
	fps = getAverageFps();
    
    mDrawUpdate->render(mFbo);
    mFbo->swap();
    
    // write file
    if(frameCount++ < 960) {
        string fileName = "particle" + std::to_string(frameCount) + ".exr";
        writeImage(
               getAppPath() / fs::path(fileName),
               mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT0)->createSource(),
               cinder::ImageTarget::Options().quality(1.0f),
               "exr"
        );
    }

}

void ParticleExport01App::draw()
{
	gl::clear();

    gl::setMatrices(mCamera);
	alfrid::helpers::drawAxis();
	alfrid::helpers::drawDotPlanes();
    
    mDrawRender->render(mFbo->read());
    
    
    // debugging
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = Config::getInstance().NUM_PARTICLES ;
    gl::draw( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT0), Rectf( 0, 0, s, s ) );
    
    gl::draw( mFbo->read()->getTexture2d(GL_COLOR_ATTACHMENT2), Rectf( s, 0, s * 2, s ) );

    // gui
	mParams->draw();
}

void ParticleExport01App::resize() {
	mCamera.setPerspective(75, getWindowAspectRatio(), .1f, 100.0f);
}

CINDER_APP( ParticleExport01App, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
