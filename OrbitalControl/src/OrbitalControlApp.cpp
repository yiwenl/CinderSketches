#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
//#include "cinder/CameraUi.h"
#include "BatchHelpers.h"
#include "OrbitalControl.hpp"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class OrbitalControlApp : public App {
public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

private:
	// camera
	CameraPersp			mCamera;
//	CameraUi			mCamUi;
    OrbitalControl*     mOrbControl;

	// helpers
	BatchAxisRef		bAxis;
	BatchGridDotsRef	bDots;
    
    // test
    float testValue = 0.0f;
    params::InterfaceGlRef mParams;
};

void OrbitalControlApp::setup()
{
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setFrameRate(60.0f);

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 10.0f);
	mCamera.lookAt(vec3(0, 0, 5), vec3(0));
    mOrbControl = new alfrid::OrbitalControl(&mCamera, getWindow());
    mOrbControl->rx->setValue(0.5);
    mOrbControl->ry->setValue(0.5);
//	mCamUi = CameraUi(&mCamera, getWindow());
    
    mCamera.getEyePoint();

	// helpers
	bAxis = BatchAxis::create();
	bDots = BatchGridDots::create();
    
    
    // test
    mParams = params::InterfaceGl::create("Orbital Control", ivec2(200, 200));
    mParams->addParam("Test Value", &testValue).min(0).max(1).step(0.001);
}

void OrbitalControlApp::keyDown( KeyEvent event )
{
	if (event.getCode() == 27) {
		quit();
	}

	if (event.getChar() == 'f') {
		setFullScreen(!isFullScreen());
	}
    
}

void OrbitalControlApp::update()
{

}

void OrbitalControlApp::draw()
{
	gl::clear();

    gl::setMatrices(mCamera);
	bAxis->draw();
	bDots->draw();
    
    mParams->draw();
}

CINDER_APP( OrbitalControlApp, RendererGl )
