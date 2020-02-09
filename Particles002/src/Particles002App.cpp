#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

#include "BatchHelpers.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class Particles002App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

    void initParticles();
    void updateShadowMap();
    
    
private:
    CameraPersp             mCam;
    CameraPersp             mCamLight;
    CameraUi                mCamUi;
    
    
    // helpers
    BatchAxisRef            bAxis;
    BatchGridDotsRef        bDots;
    BatchBallRef            bBall;
    
    // shaders
    gl::GlslProgRef         mShaderUpdate;
    gl::GlslProgRef         mShaderRender;
    
    // particles
    gl::VaoRef              mAttributes[2];
    gl::VboRef              mParticleBuffer[2];
    
    std::uint32_t           mSourceIndex        = 0;
    std::uint32_t           mDestinationIndex   = 1;
    
    
    gl::FboRef              mFbo;
    gl::Texture2dRef        mShadowMapTex;
    gl::Texture2dRef        mParticleTex;
    gl::Texture2dRef        mColorTex;
    
    vec3                    mLightPos;
    float                   mSeed = randFloat(1000.0f);
};

const int    NUM_PARTICLES = 50e4;
const int    FBO_WIDTH = 2048;
const int    FBO_HEIGHT = 2048;

struct Particle
{
    vec3    pos;
    vec3    vel;
    vec3    posOrg;
    vec3    random;
    float   life;
};


void prepareSettings( Particles002App::Settings *settings) {
    settings->setWindowSize(1920, 1080);
//    settings->setWindowSize(1280, 720);
//    settings->setWindowSize(1080 * 0.8, 1350 * 0.8);
    
    settings->setHighDensityDisplayEnabled(); // try removing this line
    settings->setMultiTouchEnabled( false );
}


void Particles002App::setup()
{
    setFrameRate(60.0f);
    gl::enableDepth();
    gl::enable( GL_POINT_SPRITE_ARB ); // or use: glEnable
    gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );   // or use: glEnable
    
    mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 10.0f );
    mCam.lookAt( vec3( 0.0, 0.0, 1.0), vec3( 0.0f ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    // helpers
    bAxis = BatchAxis::create();
    bDots = BatchGridDots::create();
    bBall = BatchBall::create();
    
    
    mParticleTex = gl::Texture2d::create( loadImage( loadAsset( "particle.png" )));
    mColorTex = gl::Texture2d::create( loadImage( loadAsset( "image.jpg" )));
//    mColorTex = gl::Texture2d::create( loadImage( loadAsset( "007.png" )));
    
    initParticles();
}

void Particles002App::initParticles() {
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );

    
    
    for( int i =0; i<particles.size(); i++) {
        float x = randFloat(-0.1, 0.1);
        float y = randFloat(-0.1, 0.1);
        float z = randFloat(-0.01, 0.01);
        
        auto &p = particles.at( i );
        
        p.pos = vec3(x, y, z);
        p.posOrg = vec3(x, y, z);
        p.life = Rand::randFloat(0.01f, 1.0f);
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
        gl::enableVertexAttribArray( 3 );
        gl::enableVertexAttribArray( 4 );
        gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
        gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel) );
        gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, posOrg) );
        gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, random) );
        gl::vertexAttribPointer( 4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, life) );
        
    }
    
    
    
    mShaderRender = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vert" ) ).fragment( loadAsset("render.frag"))
                                       .attribLocation( "ciPosition", 0 )
                                       .attribLocation( "iPositionOrg", 2 )
                                       .attribLocation( "iRandom", 3 )
                                       .attribLocation( "iLife", 4 )
                                    );
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) )
                                    .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
                                    .feedbackVaryings( { "position", "velocity", "positionOrg", "random", "life"} )
                                    .attribLocation( "iPosition", 0 )
                                    .attribLocation( "iVelocity", 1 )
                                    .attribLocation( "iPositionOrg", 2 )
                                    .attribLocation( "iRandom", 3 )
                                    .attribLocation( "iLife", 4 )
                                    );
    
    
    // shadow mapping
    float scale = 0.1f;
    mLightPos = vec3( 2.0f * scale, 10.0f * scale, 4.0f * scale);
    gl::Texture2d::Format depthFormat;
    depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
    depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
    depthFormat.setMagFilter( GL_LINEAR );
    depthFormat.setMinFilter( GL_LINEAR );
    depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    depthFormat.setCompareFunc( GL_LEQUAL );
    mShadowMapTex = gl::Texture2d::create( FBO_WIDTH, FBO_HEIGHT, depthFormat );
    
    gl::Fbo::Format fboFormat;
    fboFormat.attachment( GL_DEPTH_ATTACHMENT, mShadowMapTex );
    mFbo = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormat );
    
    // Set up camera from the light's viewpoint
    mCamLight.setPerspective( 100.0f, mFbo->getAspectRatio(), 0.5f, 20.0f );
    mCamLight.lookAt( mLightPos, mLightPos * vec3( 1.0, 0.0, 1.0f ) );
}

void Particles002App::mouseDown( MouseEvent event )
{
}

void Particles002App::update()
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


void Particles002App::updateShadowMap() {
    gl::ScopedFramebuffer fbo( mFbo );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
    
    gl::clear( Color( 0, 0, 0 ) );
    //    gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 10000.0f );
    gl::setMatrices( mCamLight );


    gl::ScopedGlslProg prog( mShaderRender );
    
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

void Particles002App::draw()
{
    updateShadowMap();
    float g = 0.0f;
	gl::clear( Color( g, g, g ) );
    gl::ScopedMatrices mcp;
    gl::setMatrices( mCam );
    
//    bAxis->draw();
//    bDots->draw(0.1);
//    bBall->draw(mLightPos, vec3(0.1f), vec3(1.0, 1.0, 0.0));
    
    gl::translate(vec3(0.0, 0.0, -0.5));
    mat4 shadowMatrix = mCamLight.getProjectionMatrix() * mCamLight.getViewMatrix();
    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    mShaderRender->uniform("uShadowMatrix", shadowMatrix);
    
    gl::ScopedTextureBind texScope( mShadowMapTex, (uint8_t) 0 );
    mShaderRender->uniform( "uShadowMap", 0 );
    gl::ScopedTextureBind texParticle( mParticleTex, (uint8_t) 1 );
    mShaderRender->uniform( "uParticleMap", 1 );
    gl::ScopedTextureBind texColor( mColorTex, (uint8_t) 2 );
    mShaderRender->uniform( "uColorMap", 2 );
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
    
//    gl::setMatricesWindow( toPixels( getWindowSize() ) );
//    int s = 128 * 2;
//    gl::draw( mFbo->getDepthTexture(), Rectf( 0, 0, s, s ) );
    
}

CINDER_APP( Particles002App, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
