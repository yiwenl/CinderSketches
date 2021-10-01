#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "BatchHelpers.h"
#include "cinder/Log.h"
#include "cinder/osc/Osc.h"

#include "OrbitalControl.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

#define USE_UDP 1

#if USE_UDP
using Sender = osc::SenderUdp;
#else
using Sender = osc::SenderTcp;
#endif

const std::string destinationHost = "192.168.1.87";
const uint16_t destinationPort = 10001;
const uint16_t localPort = 10000;


class OscSenderApp : public App {
public:
    OscSenderApp();
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
    void onSendError( asio::error_code error );
    
    Sender    mSender;
    bool      mIsConnected;

private:
	// camera
	CameraPersp			mCamera;
	CameraUi			mCamUi;

	// helpers
	BatchAxisRef		bAxis;
	BatchGridDotsRef	bDots;
    
    
    OrbitalControl*     mOrbControl;
};

OscSenderApp::OscSenderApp()
: mSender( localPort, destinationHost, destinationPort ), mIsConnected( false )
{
}


void OscSenderApp::setup()
{
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setFrameRate(60.0f);

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 20.0f);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
//	mCamUi = CameraUi(&mCamera, getWindow());
    mOrbControl = new OrbitalControl(&mCamera, getWindow());

	// helpers
	bAxis = BatchAxis::create();
	bDots = BatchGridDots::create();
    
    
    // osc
    try {
            // Bind the sender to the endpoint. This function may throw. The exception will
            // contain asio::error_code information.
            mSender.bind();
        }
        catch ( const osc::Exception &ex ) {
            CI_LOG_E( "Error binding: " << ex.what() << " val: " << ex.value() );
            quit();
        }
        
    #if ! USE_UDP
        mSender.connect(
        // Set up the OnConnectFn. If there's no error, you can consider yourself connected to
        // the endpoint supplied.
        [&]( asio::error_code error ){
            if( error ) {
                CI_LOG_E( "Error connecting: " << error.message() << " val: " << error.value() );
                quit();
            }
            else {
                CI_LOG_V( "Connected" );
                mIsConnected = true;
            }
        });
    #else
        // Udp doesn't "connect" the same way Tcp does. If bind doesn't throw, we can
        // consider ourselves connected.
        mIsConnected = true;
    #endif
}

void OscSenderApp::keyDown( KeyEvent event )
{
	if (event.getCode() == 27) {
		quit();
	}

	if (event.getChar() == 'f') {
		setFullScreen(!isFullScreen());
	}
}

void OscSenderApp::update()
{
    vec3 camPos = mCamera.getEyePoint();
    
    
    if(!mIsConnected) {
        return;
    }
    osc::Message msg( "/cameraPos" );
        msg.append( camPos.x );
        msg.append( camPos.y );
        msg.append( camPos.z );
        // Send the msg and also provide an error handler. If the message is important you
        // could store it in the error callback to dispatch it again if there was a problem.
        mSender.send( msg, std::bind( &OscSenderApp::onSendError, this, std::placeholders::_1 ) );
    
}

// Unified error handler. Easiest to have a bound function in this situation,
// since we're sending from many different places.
void OscSenderApp::onSendError( asio::error_code error )
{
    if( error ) {
        CI_LOG_E( "Error sending: " << error.message() << " val: " << error.value() );
        // If you determine that this error is fatal, make sure to flip mIsConnected. It's
        // possible that the error isn't fatal.
        mIsConnected = false;
        try {
#if ! USE_UDP
            // If this is Tcp, it's recommended that you shutdown before closing. This
            // function could throw. The exception will contain asio::error_code
            // information.
            mSender.shutdown();
#endif
            // Close the socket on exit. This function could throw. The exception will
            // contain asio::error_code information.
            mSender.close();
        }
        catch( const osc::Exception &ex ) {
            CI_LOG_EXCEPTION( "Cleaning up socket: val -" << ex.value(), ex );
        }
        quit();
    }
}

void OscSenderApp::draw()
{
	gl::clear();

    gl::setMatrices(mCamera);
	bAxis->draw();
	bDots->draw();
}

CINDER_APP( OscSenderApp, RendererGl )
