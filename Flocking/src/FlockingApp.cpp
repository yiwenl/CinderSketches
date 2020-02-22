#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

#include "BatchHelpers.h"



using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_PARTICLES = 10e2;


struct Particle
{
    vec3    pos;
    vec3    vel;
    vec3    random;
};


class FlockingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
    void                    initParticles();
    
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    // helpers
    BatchAxisRef            bAxis;
    BatchGridDotsRef        bDots;
    
    
    // shaders
    gl::GlslProgRef         mShaderUpdate;
    gl::GlslProgRef         mShaderRender;
    
    
    // particles
    gl::VaoRef              mAttributes[2];
    gl::VboRef              mParticleBuffer[2];
    
    std::uint32_t           mSourceIndex        = 0;
    std::uint32_t           mDestinationIndex   = 1;
    
    float mSeed = randFloat(10000.0f);
};

void prepareSettings( FlockingApp::Settings *settings) {
//    settings->setWindowSize(1920, 1080);
    settings->setWindowSize(1280, 720);
//    settings->setWindowSize(1080 * 0.8, 1350 * 0.8);
    
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}


void FlockingApp::setup()
{
    setFrameRate(60.0f);
    gl::enableDepth();
    gl::enable( GL_POINT_SPRITE_ARB ); // or use: glEnable
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );   // or use: glEnable
    
    
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 50.0f );
    mCam.lookAt( vec3( -5.0, 5.0, 5.0), vec3( 0.0f ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    // helpers
    bAxis = BatchAxis::create();
    bDots = BatchGridDots::create();
    
    initParticles();
}

void FlockingApp::initParticles()
{
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );

    float r = 5.0;
    
    for( int i =0; i<particles.size(); i++) {
        vec3 v = randVec3() * randFloat(r);
        
        auto &p = particles.at( i );
        
        p.pos = v;
        p.random = vec3(randFloat(), randFloat(), randFloat());
    }
    
    mParticleBuffer[mSourceIndex]       = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW );
    mParticleBuffer[mDestinationIndex]  = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW );
    
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
        gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel) );
        gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, random) );
        
    }
    
    
    
    mShaderRender = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vert" ) ).fragment( loadAsset("render.frag"))
                                       .attribLocation( "ciPosition", 0 )
                                       .attribLocation( "iRandom", 2 )
                                    );
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) )
                                    .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
                                    .feedbackVaryings( { "position", "velocity", "random"} )
                                    .attribLocation( "iPosition", 0 )
                                    .attribLocation( "iVelocity", 1 )
                                    .attribLocation( "iRandom", 2 )
                                    );
}

void FlockingApp::mouseDown( MouseEvent event )
{
}

void FlockingApp::update()
{
    gl::ScopedGlslProg prog( mShaderUpdate );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage
    mShaderUpdate->uniform("uTime", float(getElapsedSeconds()) + mSeed);
    
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    std::swap( mSourceIndex, mDestinationIndex );
}

void FlockingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedMatrices mcp;
    gl::setMatrices(mCam);
    
    bAxis->draw();
    bDots->draw();
    
    
    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));   
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

CINDER_APP( FlockingApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
