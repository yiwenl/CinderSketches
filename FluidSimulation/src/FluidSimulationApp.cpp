#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "FluidSim.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class FluidSimulationApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    
private:
    FluidSim* fluid;
};

void prepareSettings( FluidSimulationApp::Settings *settings) {
    settings->setWindowSize(1280, 720);
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}

void FluidSimulationApp::setup()
{
    setFrameRate(60.0f);
    
    fluid = new FluidSim(128);
    fluid->timestep *= 0.5;
}

void FluidSimulationApp::mouseDown( MouseEvent event )
{
}

void FluidSimulationApp::update()
{
    float y = sin(getElapsedSeconds());
    fluid->updateFlow(vec2(0.25, 0.5), vec2(1.0, y), 5, 4, randFloat(0.4, 0.6));
    
    y = cos(getElapsedSeconds() * 0.7653784);
    fluid->updateFlow(vec2(0.75, 0.5), vec2(-1.0, y), 3, 3, randFloat(0.4, 0.6));
    
    fluid->update();
}

void FluidSimulationApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = 512;
    gl::draw( fluid->getVelocity(), Rectf( 0, 0, s, s ) );
    gl::draw( fluid->getDensity(), Rectf( s, 0, s * 2, s ) );
}

CINDER_APP( FluidSimulationApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
