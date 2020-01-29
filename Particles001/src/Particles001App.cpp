#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cmath"

using namespace ci;
using namespace ci::app;
using namespace std;

class Particles001App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    private :
        gl::GlslProgRef mRenderProg;
        gl::GlslProgRef mUpdateProg;
    
    // Descriptions of particle data layout.
    gl::VaoRef        mAttributes[2];
    // Buffers holding raw particle data on GPU.
    gl::VboRef        mParticleBuffer[2];
    
    
    // Current source and destination buffers for transform feedback.
    // Source and destination are swapped each frame after update.
    std::uint32_t    mSourceIndex        = 0;
    std::uint32_t    mDestinationIndex    = 1;
};


struct Particle
{
    vec3    pos;
    vec3    posOrg;
    float   life;
};

const int NUM_PARTICLES = 1024 * 1024;


void prepareSettings( Particles001App::Settings *settings) {
//    settings->setWindowSize(1920, 1080);
    settings->setWindowSize(1280, 720);
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}

void Particles001App::setup()
{
    console() << "Number of particles :  " << NUM_PARTICLES << endl;
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );
    
    vec3 center = vec3(getWindowCenter(), 0.0f);
    for( int i =0; i<particles.size(); i++) {
        float a = randFloat() * M_PI * 2.0;
        float r = randFloat(100.0, 120.0);
        float x = cos(a) * r;
        float y = sin(a) * r;
        float z = randFloat(-0.1f, 0.1f);
        auto &p = particles.at( i );
        
        p.pos = center + vec3(x, y, z);
        p.posOrg = center + vec3(x, y, z);
        p.life = randFloat(0.5f, 1.0f);
    }
    
    mParticleBuffer[mSourceIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW );
    mParticleBuffer[mDestinationIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW );
    
    for( int i = 0; i < 2; ++i )
    {    // Describe the particle layout for OpenGL.
        mAttributes[i] = gl::Vao::create();
        gl::ScopedVao vao( mAttributes[i] );
        
        // Define attributes as offsets into the bound particle buffer
        gl::ScopedBuffer buffer( mParticleBuffer[i] );
        gl::enableVertexAttribArray( 0 );
        gl::enableVertexAttribArray( 1 );
        gl::enableVertexAttribArray( 2 );
        gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
        gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, posOrg) );
        gl::vertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, life) );
        
    }
    
    
    mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vert" ) ).fragment( loadAsset("render.frag")));
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
    gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 10000.0f );
    gl::enableDepthRead();
    gl::enableDepthWrite();

    gl::ScopedGlslProg render( mRenderProg );
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

CINDER_APP( Particles001App, RendererGl, prepareSettings )
