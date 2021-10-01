#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Pixelate00App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void Pixelate00App::setup()
{
}

void Pixelate00App::mouseDown( MouseEvent event )
{
}

void Pixelate00App::update()
{
}

void Pixelate00App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( Pixelate00App, RendererGl )
