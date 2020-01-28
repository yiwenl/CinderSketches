#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Particles001App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};


void prepareSettings( Particles001App::Settings *settings) {
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}

void Particles001App::setup()
{
}

void Particles001App::mouseDown( MouseEvent event )
{
}

void Particles001App::update()
{
}

void Particles001App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( Particles001App, RendererGl, prepareSettings )
