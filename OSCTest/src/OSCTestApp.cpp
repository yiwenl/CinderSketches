#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Log.h"
#include "BatchHelpers.h"
#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

#define USE_UDP 1

#if USE_UDP
using Receiver = osc::ReceiverUdp;
using protocol = asio::ip::udp;
#else
using Receiver = osc::ReceiverTcp;
using protocol = asio::ip::tcp;
#endif

const uint16_t localPort = 3333;

class OSCTestApp : public App {
public:
    OSCTestApp();
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
    
    Receiver mReceiver;
    std::map<uint64_t, protocol::endpoint> mConnections;
};

OSCTestApp::OSCTestApp()
: mReceiver( localPort )
{
}


void OSCTestApp::setup()
{
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setFrameRate(60.0f);

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 50.0f);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
//	mCamUi = CameraUi(&mCamera, getWindow());

	// helpers
	bAxis = BatchAxis::create();
	bDots = BatchGridDots::create();
    
    // osc
    mReceiver.setListener( "/cameraPos",
    [&]( const osc::Message &msg ){
        float x = msg[0].flt();
        float y = msg[1].flt();
        float z = msg[2].flt();
        
        mCamera.lookAt(vec3(x, y, z), vec3(0));
    });
    
    
    
    try {
        // Bind the receiver to the endpoint. This function may throw.
        mReceiver.bind();
    }
    catch( const osc::Exception &ex ) {
        CI_LOG_E( "Error binding: " << ex.what() << " val: " << ex.value() );
        quit();
    }
    
        
    #if USE_UDP
        // UDP opens the socket and "listens" accepting any message from any endpoint. The listen
        // function takes an error handler for the underlying socket. Any errors that would
        // call this function are because of problems with the socket or with the remote message.
        mReceiver.listen(
        []( asio::error_code error, protocol::endpoint endpoint ) -> bool {
            if( error ) {
                CI_LOG_E( "Error Listening: " << error.message() << " val: " << error.value() << " endpoint: " << endpoint );
                return false;
            }
            else
                return true;
        });
    #else
        mReceiver.setConnectionErrorFn(
        // Error Function for Accepted Socket Errors. Will be called anytime there's an
        // error reading from a connected socket (a socket that has been accepted below).
        [&]( asio::error_code error, uint64_t identifier ) {
            if ( error ) {
                auto foundIt = mConnections.find( identifier );
                if( foundIt != mConnections.end() ) {
                    // EOF or end of file error isn't specifically an error. It's just that the
                    // other side closed the connection while you were expecting to still read.
                    if( error == asio::error::eof ) {
                        CI_LOG_W( "Other side closed the connection: " << error.message() << " val: " << error.value() << " endpoint: " << foundIt->second.address().to_string()
                                 << " port: " << foundIt->second.port() );
                    }
                    else {
                        CI_LOG_E( "Error Reading from Socket: " << error.message() << " val: "
                             << error.value() << " endpoint: " << foundIt->second.address().to_string()
                             << " port: " << foundIt->second.port() );
                    }
                    mConnections.erase( foundIt );
                }
            }
        });
        auto expectedOriginator = protocol::endpoint( asio::ip::address::from_string( "127.0.0.1" ), 10000 );
        mReceiver.accept(
        // Error Handler for the acceptor. You'll return true if you want to continue accepting
        // or fals otherwise.
        []( asio::error_code error, protocol::endpoint endpoint ) -> bool {
            if( error ) {
                CI_LOG_E( "Error Accepting: " << error.message() << " val: " << error.value()
                         << " endpoint: " << endpoint.address().to_string() );
                return false;
            }
            else
                return true;
        },
        // Accept Handler. Return whether or not the acceptor should cache this connection
        // (true) or dismiss it (false).
        [&, expectedOriginator]( osc::TcpSocketRef socket, uint64_t identifier ) -> bool {
            // Here we return whether or not the remote endpoint is the expected endpoint
            mConnections.emplace( identifier, socket->remote_endpoint() );
            return socket->remote_endpoint() == expectedOriginator;
        } );
    #endif

}

void OSCTestApp::keyDown( KeyEvent event )
{
	if (event.getCode() == 27) {
		quit();
	}

	if (event.getChar() == 'f') {
		setFullScreen(!isFullScreen());
	}
}

void OSCTestApp::update()
{

}

void OSCTestApp::draw()
{
	gl::clear();

    gl::setMatrices(mCamera);
	bAxis->draw();
	bDots->draw();
}

CINDER_APP( OSCTestApp, RendererGl )
