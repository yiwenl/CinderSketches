#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "FboPingPong.hpp"
#include "Draw.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

const int WINDOW_WIDTH = 480;
const int WINDOW_HEIGHT = 640;

class PostProcessingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    gl::Texture2dRef mTexture;
    FboPingPong* mFbos;
    Draw* mDraw;
};

void PostProcessingApp::setup()
{
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    DisplayRef display = Display::getMainDisplay();
    ivec2 displaySize = display->getSize();
    
    setWindowPos((displaySize.x - WINDOW_WIDTH)/2, (displaySize.y - WINDOW_HEIGHT)/2);
    
    mTexture = gl::Texture2d::create( loadImage( loadAsset("test.jpg") ) );
    mFbos = new FboPingPong(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    auto plane = gl::VboMesh::create( geom::Plane().normal(vec3(0, 0, 1)) );
    
    mDraw = new Draw();
    mDraw->setMesh(plane)
        ->useProgram("test.vert", "test.frag");
    
    gl::ScopedFramebuffer fbo(mFbos->read());
    gl::clear( Color( 0, 0, 0 ) );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbos->read()->getSize() );
    mDraw->bindTexture("uMap", mTexture, 0)
        ->uniform("uOffset", 0.0f)
        ->draw();
    
}

void PostProcessingApp::mouseDown( MouseEvent event )
{
}

void PostProcessingApp::update()
{
    gl::ScopedFramebuffer fbo(mFbos->write());
    gl::clear( Color( 0, 0, 0 ) );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbos->write()->getSize() );
    mDraw->bindTexture("uMap", mFbos->read()->getColorTexture(), 0)
        ->uniform("uOffset", 0.002f)
        ->draw();
    
    mFbos->swap();
    
}

void PostProcessingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::draw(mFbos->read()->getColorTexture());
}

CINDER_APP( PostProcessingApp, RendererGl,
[]( PostProcessingApp::Settings *settings )
{
    settings->setHighDensityDisplayEnabled( true );
} )
