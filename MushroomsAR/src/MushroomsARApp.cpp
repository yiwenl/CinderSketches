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

const int    NUM_PARTICLES = 100e3;
const int    FBO_WIDTH = 2048;
const int    FBO_HEIGHT = 2048;

class MushroomsARApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session     mARSession;
    
    
    
private:
    void updateShadowMap();
    void initParticles();
    
    // camera
    CameraPersp             mCamLight;
        
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
    mat4                    mMtxModel;
    bool                    mTouched = false;
    bool                    mHasMtxModel = false;
    float                   mSeed = randFloat(1000.0f);
    
    float mOffset       = 0.0;
    float mTargetOffset = 0.0;
    float mParticleSize = 1.0;
    float mTargetParticleSize = 1.0;
    int   mState        = 0;
    
    float mStartTime    = 0.0f;
    
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
    gl::enableDepth();
    
    auto config = ARKit::SessionConfiguration()
                        .imageTrackingEnabled( true )
                        .trackingType( ARKit::TrackingType::WorldTracking );
    mARSession.runConfiguration( config );
   
    mParticleTex = gl::Texture2d::create( loadImage( loadAsset( "particle.png" )));
    mColorTex = gl::Texture2d::create( loadImage( loadAsset( "image.jpg" )));
    
    initParticles();
}

void MushroomsARApp::initParticles() {
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );
    
    float w = 0.2 * 0.5;
    float h = 0.294 * 0.5;;
    
    console() << " size : " << mColorTex->getSize() << endl;
    
    
    for( int i =0; i<particles.size(); i++) {
        float x = randFloat(-w, w);
        float y = randFloat(-h, h);
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
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) ).fragment( loadAsset("no_op_es3.frag"))
                                    .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
                                    .feedbackVaryings( { "position", "velocity", "positionOrg", "random", "life"} )
                                    .attribLocation( "iPosition", 0 )
                                    .attribLocation( "iVelocity", 1 )
                                    .attribLocation( "iPositionOrg", 2 )
                                    .attribLocation( "iRandom", 3 )
                                    .attribLocation( "iLife", 4 )
                                    );
    
    
    // shadow mapping
    float scale = 0.05f;
    mLightPos = vec3( -5.0f * scale, -7.0f * scale, 0.0f * scale);
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
    mCamLight.setPerspective( 100.0f, mFbo->getAspectRatio(), 0.1f, 10.0f );
    mCamLight.lookAt( mLightPos, vec3(0.0, 0.0, -0.2));
}


void MushroomsARApp::touchesBegan( TouchEvent event )
{
    
    
    mTouched = true;
    mState++;
    if(mState == 1) {
        console() << " Hide Particles " << endl;
        mParticleSize = 0.0;
        mTargetParticleSize = 0.0;
    } else if (mState == 2 ) {
        mTargetParticleSize = 1.0;
        console() << "Start particle movement" << endl;
        mStartTime = (float)getElapsedSeconds();
    }
    
    console() << "State : " << mState << endl;
}

void MushroomsARApp::update()
{
    if(randFloat() > 0.9) {
        console() << getFrameRate() << endl;
    }
    if(mState >= 2) {
        float elapsedTime = getElapsedSeconds() - mStartTime;
        
        if(elapsedTime > 0.5) {
            if(mTargetOffset < 0.01) {
                console() << " Start moving " << endl;
            }
            mTargetOffset = 1.0;
            
        }
    }
    
    mOffset += (mTargetOffset - mOffset) * 0.05;
    mParticleSize += (mTargetParticleSize - mParticleSize) * 0.05;
    gl::ScopedGlslProg prog( mShaderUpdate );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage
    mShaderUpdate->uniform("uTime", float(getElapsedSeconds()) + mSeed);
    mShaderUpdate->uniform("uOffset", mOffset);
    
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    std::swap( mSourceIndex, mDestinationIndex );
}

void MushroomsARApp::updateShadowMap()
{
    gl::ScopedFramebuffer fbo( mFbo );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
    gl::ScopedMatrices matScp;
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices( mCamLight );

    gl::ScopedGlslProg prog( mShaderRender );

    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    mShaderRender->uniform("uParticleSize", mParticleSize);
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
    
}

void MushroomsARApp::draw()
{
    updateShadowMap();
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    
    gl::enableDepth();
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );


    for (const auto& a : mARSession.getImageAnchors())
    {
        gl::ScopedMatrices matScp;
        
        if(mTouched) {
            if(!mHasMtxModel) {
                mMtxModel = mat4(a.mTransform);
                mHasMtxModel = true;
                console() << " Get Model Matrix: " << mMtxModel << endl;
            }
            gl::setModelMatrix( mMtxModel );
        } else {
            gl::setModelMatrix( a.mTransform );
        }
        
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground

        
        mat4 shadowMatrix = mCamLight.getProjectionMatrix() * mCamLight.getViewMatrix();
        gl::ScopedGlslProg prog( mShaderRender );
        mShaderRender->uniform("uViewport", vec2(getWindowSize()));
        mShaderRender->uniform("uShadowMatrix", shadowMatrix);
        mShaderRender->uniform("uParticleSize", mParticleSize);
        
        gl::ScopedTextureBind texScope( mShadowMapTex, (uint8_t) 0 );
        mShaderRender->uniform( "uShadowMap", 0 );
        gl::ScopedTextureBind texParticle( mParticleTex, (uint8_t) 1 );
        mShaderRender->uniform( "uParticleMap", 1 );
        gl::ScopedTextureBind texColor( mColorTex, (uint8_t) 2 );
        mShaderRender->uniform( "uColorMap", 2 );
        
        gl::ScopedVao vao( mAttributes[mSourceIndex] );
        gl::context()->setDefaultShaderVars();
        gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
    }

    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = 128 * 2;
    gl::draw( mFbo->getDepthTexture(), Rectf( 0, 100, s, s+100 ) );
}

CINDER_APP( MushroomsARApp, RendererGl )
