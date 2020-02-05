#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "CinderARKit.h"
#include "cinder/Rand.h"
#include "Config.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

const int    FBO_WIDTH = 2048;
const int    FBO_HEIGHT = 2048;

class MushroomsARApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session     mARSession;
    
    gl::Texture2dRef   mTexture;
    gl::Texture2dRef   mTextureParticle;
    gl::Texture2dRef   mShadowMapTex;
    
    gl::BatchRef        mBox;
    gl::GlslProgRef     mShaderColor;
    gl::GlslProgRef     mShaderRender;
    gl::GlslProgRef     mShaderUpdate;
    
    
    // Descriptions of particle data layout.
    gl::VaoRef          mAttributes[2];
    // Buffers holding raw particle data on GPU.
    gl::VboRef          mParticleBuffer[2];
    
    std::uint32_t       mSourceIndex        = 0;
    std::uint32_t       mDestinationIndex   = 1;
    
    gl::FboRef          mFbo;
    
    CameraPersp         mCamLight;
    CameraOrtho         mCamParticle;
    vec3                mLightPos;
    
    float               mSeed = randFloat(1000.0f);
    float               mOffset = 0.0;
    float               mTargetOffset = 0.0;
    
private:
    void updateShadowMap();
    void initParticles();
    void initShadowMap();
    
};

struct Particle
{
    vec3    pos;
    vec3    vel;
    vec3    posOrg;
    vec3    random;
    float   life;
};



void MushroomsARApp::setup()
{
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    auto config = ARKit::SessionConfiguration()
                        .imageTrackingEnabled( true )
                        .trackingType( ARKit::TrackingType::WorldTracking );
    mARSession.runConfiguration( config );
    
    // This texture is just used for visualisation
    mTexture = gl::Texture2d::create( loadImage( loadAsset( "image.jpg" )));
    mTextureParticle = gl::Texture2d::create( loadImage( loadAsset( "particle.png" )));
    
    
//    mShaderColor = gl::GlslProg::create( loadAsset( "basic.vert" ), loadAsset( "color.frag" ) );
//    auto box = gl::VboMesh::create( geom::Cube().size(1.0f, 1.0f, 1.0f) );
//    mBox = gl::Batch::create(box, mShaderColor);
    
    
    initParticles();
    initShadowMap();
}

void MushroomsARApp::initParticles() {
    // init particles
    console() << "Number of particles : " << Config::getInstance().NUM_PARTICLES << endl;
    vector<Particle> particles;
    particles.assign( Config::getInstance().NUM_PARTICLES, Particle() );
    float ratio = (float)mTexture->getWidth() / (float)mTexture->getHeight();
    float rangeX = 0.1;
    float rangeY = rangeX/ ratio;
    float rangeZ = .01;
    
    
    
    for( int i =0; i<particles.size(); i++) {
        float x = randFloat(-rangeX, rangeX);
        float y = randFloat(-rangeY, rangeY);
        float z = randFloat(-rangeZ, rangeZ);
        
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
    mShaderRender->uniform("uSize", vec2(rangeX, rangeY));
    
    
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) ).fragment( loadAsset( "no_op_es3.frag" ) )
        .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
        .feedbackVaryings( { "position", "velocity", "positionOrg", "random", "life"} )
        .attribLocation( "iPosition", 0 )
        .attribLocation( "iVelocity", 1 )
        .attribLocation( "iPositionOrg", 2 )
        .attribLocation( "iRandom", 3 )
        .attribLocation( "iLife", 4 )
        );
}

void MushroomsARApp::initShadowMap() {
    // shadow mapping
    
    float scale = 0.035f;
    mLightPos = vec3( 0.0f, 10.0f * scale, 2.0f * scale);
    console() << "Light Position : " << mLightPos << endl;
    
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
    mCamLight.setPerspective( 100.0f, mFbo->getAspectRatio(), 0.1f, 2.0f );
    mCamLight.lookAt( mLightPos, vec3( 0.0f ) );
    
}

void MushroomsARApp::touchesBegan( TouchEvent event )
{
    mTargetOffset = 1.0;
}

void MushroomsARApp::update()
{
    mOffset += (mTargetOffset - mOffset) * 0.05;
    // Update particles on the GPU
    gl::ScopedGlslProg prog( mShaderUpdate );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage

    mShaderUpdate->uniform("uTime", float(getElapsedSeconds()) * 0.5f + mSeed);
    mShaderUpdate->uniform("uOffset", mOffset);
    
    // Bind the source data (Attributes refer to specific buffers).
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    // Bind destination as buffer base.
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );

    // Draw source into destination, performing our vertex transformations.
    gl::drawArrays( GL_POINTS, 0, Config::getInstance().NUM_PARTICLES );

    gl::endTransformFeedback();

    // Swap source and destination for next loop
    std::swap( mSourceIndex, mDestinationIndex );
}

void MushroomsARApp::updateShadowMap() {
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::ScopedFramebuffer fbo( mFbo );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
    
    gl::clear( Color( 1, 1, 0 ) );
    gl::ScopedMatrices matScp;
    
    gl::setMatrices( mCamLight );
    gl::enableDepthRead();
    gl::enableDepthWrite();

    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uOffset", mOffset);
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    gl::ScopedTextureBind texMap( mTexture, (uint8_t) 0 );
    mShaderRender->uniform( "uColorMap", 0 );
    
    gl::ScopedTextureBind texParticle( mTextureParticle, (uint8_t) 1 );
    mShaderRender->uniform( "uParticleMap", 1 );
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, Config::getInstance().NUM_PARTICLES );
    
}

void MushroomsARApp::draw()
{
    updateShadowMap();
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();

    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
   
       
    for (const auto& a : mARSession.getImageAnchors())
    {
        gl::ScopedMatrices matScp;
        gl::setModelMatrix( a.mTransform );
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
        
        gl::ScopedGlslProg progRender( mShaderRender );
        mat4 shadowMatrix = mCamLight.getProjectionMatrix() * mCamLight.getViewMatrix();
        mShaderRender->uniform("uShadowMatrix", shadowMatrix);
        mShaderRender->uniform("uOffset", mOffset);
        
        mShaderRender->uniform( "uViewport", vec2(getWindowSize()));
        
        gl::ScopedTextureBind texMap( mTexture, (uint8_t) 0 );
        mShaderRender->uniform( "uColorMap", 0 );
        
        gl::ScopedTextureBind texParticle( mTextureParticle, (uint8_t) 1 );
        mShaderRender->uniform( "uParticleMap", 1 );
        
        gl::ScopedTextureBind texShadow( mShadowMapTex, (uint8_t) 2 );
        mShaderRender->uniform( "uShadowMap", 2 );
        
        gl::ScopedVao vao( mAttributes[mSourceIndex] );
        gl::context()->setDefaultShaderVars();
        gl::drawArrays( GL_POINTS, 0, Config::getInstance().NUM_PARTICLES );
    }
   
    
    
//    gl::setMatricesWindow( toPixels( getWindowSize() ) );
//    int s = 128 * 2;
//    gl::draw( mFbo->getDepthTexture(), Rectf( 0, 100, s, s+100 ) );
//    gl::draw( mFbo->getColorTexture(), Rectf( s, 100, s * 2, s+100 ) );
}

CINDER_APP( MushroomsARApp, RendererGl )
