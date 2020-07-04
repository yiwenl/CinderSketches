#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"

#include "BatchHelpers.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Particles003App : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

private:
	// camera
	CameraPersp			mCamera;
	CameraUi			mCamUi;

	// helpers
	BatchAxisRef		bAxis;
	BatchGridDotsRef	bDots;
};

void Particles003App::setup()
{
	setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setFrameRate(60.0f);

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 10.0f);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
	mCamUi = CameraUi(&mCamera, getWindow());

	// helpers
	bAxis = BatchAxis::create();
	bDots = BatchGridDots::create();

}

void Particles003App::keyDown( KeyEvent event )
{
	if (event.getCode() == 27) {
		quit();
	}

	if (event.getChar() == 'f') {
		setFullScreen(!isFullScreen());
	}
}

void Particles003App::update()
{
}

void Particles003App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::setMatrices(mCamera);
	bAxis->draw();
	bDots->draw();
}

CINDER_APP( Particles003App, RendererGl )
