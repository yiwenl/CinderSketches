#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderARKit.h"
#include "BatchHelpers.h"
#include "Utils.hpp"

#include "cinder/Rand.h"
#include "cinder/Camera.h"


using namespace ci;
using namespace ci::app;
using namespace std;

const int    FBO_WIDTH = 2048;
const int    FBO_HEIGHT = 2048;
const int    NUM_PARTICLES = 10e4;


struct Particle
{
    vec3    pos;
    vec3    vel;
    vec3    posOrg;
    vec3    random;
    vec3    color;
    float   life;
};


class BlackHole02App : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    void updateEnvMap();
    void updateShadowMap();
    
    ARKit::Session mARSession;
    
    BatchBallRef bBall;
    
    
    gl::FboRef          mFboEnv;
    gl::FboRef          mFboShadow;
    gl::Texture2dRef    mShadowMapTex;
    
    // Descriptions of particle data layout.
    gl::VaoRef          mAttributes[2];
    // Buffers holding raw particle data on GPU.
    gl::VboRef          mParticleBuffer[2];
    
    std::uint32_t       mSourceIndex        = 0;
    std::uint32_t       mDestinationIndex   = 1;
    
    
    gl::GlslProgRef mShaderRender;
    gl::GlslProgRef mShaderUpdate;
    gl::GlslProgRef mShaderInit;
    gl::GlslProgRef mShaderFloor;
    
    
    gl::BatchRef mBatchFloor;
    
    CameraPersp     mLightCam;
    mat4            mtxShadow;
    
    vec3            mCenter = vec3(100.0);
    vec3            mPosFloor = vec3(100.0);
    
    EaseNumberRef   mOffset;
    
    float mSeed;
    
};

void BlackHole02App::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Both );
    
    mARSession.runConfiguration( config );
    
    
    bBall = BatchBall::create();
    
    mSeed = randFloat();
    
    // frame buffer
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormatEnv.colorTexture() );
    
    mOffset = EaseNumber::create(0.0f);
    
    // init particles
    
    console() << "Number of particles : " << NUM_PARTICLES << endl;
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );
    
    for( int i =0; i<particles.size(); i++) {
        float x = randFloat(-1, 1);
        float y = randFloat(-1, 1);
        float z = randFloat(-1, 1);
        
        auto &p = particles.at( i );
        
        p.pos = vec3(x, y, z);
        p.vel = vec3(0, 0, 0);
        p.posOrg = vec3(x, y, z);
        p.life = Rand::randFloat(0.01f, 1.0f);
        p.random = vec3(randFloat(), randFloat(), randFloat());
        p.color = vec3(1.0, 0.0, 0.0);
        
        if(i == 0) {
            console() << p.pos << endl;
        }
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
        gl::enableVertexAttribArray( 5 );
        gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
        gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel) );
        gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, posOrg) );
        gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, random) );
        gl::vertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color) );
        gl::vertexAttribPointer( 5, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, life) );
        
    }
        
    
    mShaderRender = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vert" ) ).fragment( loadAsset("render.frag"))
       .attribLocation( "ciPosition", 0 )
       .attribLocation( "iPositionOrg", 2 )
       .attribLocation( "iRandom", 3 )
       .attribLocation( "iColor", 4 )
       .attribLocation( "iLife", 5 )
    );
        
    mShaderInit = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "init.vert" ) ).fragment( loadAsset( "no_op_es3.frag" ) )
        .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
        .feedbackVaryings( { "position", "velocity", "positionOrg", "random", "color", "life"} )
        .attribLocation( "iPosition", 0 )
        .attribLocation( "iVelocity", 1 )
        .attribLocation( "iPositionOrg", 2 )
        .attribLocation( "iRandom", 3 )
        .attribLocation( "iColor", 4 )
        .attribLocation( "iLife", 5 )
    );
    
    
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) ).fragment( loadAsset( "no_op_es3.frag" ) )
        .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
        .feedbackVaryings( { "position", "velocity", "positionOrg", "random", "color", "life"} )
        .attribLocation( "iPosition", 0 )
        .attribLocation( "iVelocity", 1 )
        .attribLocation( "iPositionOrg", 2 )
        .attribLocation( "iRandom", 3 )
        .attribLocation( "iColor", 4 )
        .attribLocation( "iLife", 5 )
    );
    
    
    // shadow mapping
    int fboSize = 1024;
    
    gl::Texture2d::Format depthFormat;
    depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
    depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
    depthFormat.setMagFilter( GL_LINEAR );
    depthFormat.setMinFilter( GL_LINEAR );
    depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    depthFormat.setCompareFunc( GL_LEQUAL );
    mShadowMapTex = gl::Texture2d::create( fboSize, fboSize, depthFormat );
    
    gl::Fbo::Format fboFormat;
    fboFormat.attachment( GL_DEPTH_ATTACHMENT, mShadowMapTex );
    mFboShadow = gl::Fbo::create( fboSize, fboSize, fboFormat );
    
    
    // floor
    auto plane = gl::VboMesh::create( geom::Plane().normal(vec3(0, 1, 0)) );
    mShaderFloor = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "floor.vert" ) ).fragment( loadAsset("floor.frag"))
    );
    
    mBatchFloor = gl::Batch::create(plane, mShaderFloor);
}

void BlackHole02App::touchesBegan( TouchEvent event )
{
    vec3 dir = AlfridUtils::getLookDir(mARSession.getViewMatrix());
    dir.y *= 0.0;
    dir = normalize(dir);
    
    mat4 mtxProj = mARSession.getProjectionMatrix() * mARSession.getViewMatrix();
    
    
    vec3 pos = mARSession.getCameraPosition() + dir * 10.0f;
    
    // center y : 1.5
    // hole radius : 1.5
    // camera y : centerY + 4.0 = 5.5
    
    mLightCam.setPerspective( 45.0f, mFboShadow->getAspectRatio(), 2.0f, 6.0f );
    mLightCam.lookAt(pos + vec3(0.0, 4.0, 0.1), pos * vec3(1.0, 0.0, 1.0));
    mtxShadow = mLightCam.getProjectionMatrix() * mLightCam.getViewMatrix();
    
    mCenter = vec3(pos);
    mPosFloor = vec3(pos) - vec3(0.0, 2.0, 0.0);
    
    // init particles
    gl::ScopedGlslProg prog( mShaderInit );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage
    mShaderInit->uniform("uPosition", mCenter);
    mShaderInit->uniform("uShadowMatrix", mtxProj);
    
    gl::ScopedTextureBind texScope( mFboEnv->getColorTexture(), (uint8_t) 0 );
    mShaderInit->uniform( "uEnvMap", 0 );

    // Bind the source data (Attributes refer to specific buffers).
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    // Bind destination as buffer base.
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );

    // Draw source into destination, performing our vertex transformations.
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    // Swap source and destination for next loop
    std::swap( mSourceIndex, mDestinationIndex );
    
    mOffset->setValue(1.0f);
    
}

void BlackHole02App::updateEnvMap() {
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;

    mARSession.drawRGBCaptureTexture(getWindowBounds());
}

void BlackHole02App::updateShadowMap() {
    gl::enableDepth();
    gl::ScopedFramebuffer fbo( mFboShadow );
    gl::ScopedViewport viewport( vec2(0.0f), mFboShadow->getSize());
    
    gl::ScopedMatrices mcp;
    gl::clear( Color(0, 0, 0));
    gl::setMatrices(mLightCam);
    
    gl::ScopedGlslProg prog(mShaderRender);
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    mShaderRender->uniform("uOffset", mOffset->getValue());
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
    
}

void BlackHole02App::update()
{
    mOffset->update();
    
    mat4 mtxProj = mARSession.getProjectionMatrix() * mARSession.getViewMatrix();
    
    updateEnvMap();
    updateShadowMap();
    
    gl::ScopedGlslProg prog( mShaderUpdate );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage
    mShaderUpdate->uniform("uTime", float(getElapsedSeconds()) + mSeed);
    mShaderUpdate->uniform("uOffset", mOffset->getValue());
    mShaderUpdate->uniform("uSeed", mSeed);
    mShaderUpdate->uniform("uCenter", mCenter);
    mShaderUpdate->uniform("uShadowMatrix", mtxProj);
    gl::ScopedTextureBind texScope( mFboEnv->getColorTexture(), (uint8_t) 0 );
    mShaderUpdate->uniform( "uEnvMap", 0 );
        
    gl::ScopedVao source( mAttributes[mSourceIndex] );
    gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
    gl::beginTransformFeedback( GL_POINTS );
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

    gl::endTransformFeedback();

    std::swap( mSourceIndex, mDestinationIndex );
}

void BlackHole02App::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    Ray rayCam = AlfridUtils::getLookRay(mARSession.getCameraPosition(), mARSession.getViewMatrix());
    
    
    gl::enableDepth();
    bBall->draw(mCenter, vec3(0.1f), vec3(0.1));
    
    
    // render particles
    
    
    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    mShaderRender->uniform("uOffset", mOffset->getValue());
    mShaderRender->uniform("uShadowMatrix", mtxShadow);
    
    gl::ScopedTextureBind texScope( mShadowMapTex, (uint8_t) 0 );
    mShaderRender->uniform( "uShadowMap", 0 );
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
    
    gl::ScopedGlslProg progFloor( mShaderFloor );
    mShaderFloor->uniform("uPosition", mPosFloor);
    
    mShaderFloor->uniform("uShadowMatrix", mtxShadow);
    
    gl::ScopedTextureBind texScope1( mShadowMapTex, (uint8_t) 0 );
    mShaderFloor->uniform( "uShadowMap", 0 );
    
    mBatchFloor->draw();
    
/*
    gl::disableDepthRead();
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int ss = 128 * 2;
    int y = 50;
    gl::draw( mFboShadow->getColorTexture(), Rectf( 0, y, ss, ss +y ) );
    gl::draw( mShadowMapTex, Rectf( ss, y, ss * 2, ss + y ) );
*/
//       gl::draw( particleViews.at(0)->mShadowMapTex, Rectf( ss, 0, ss * 2, ss ) );
       
}

CINDER_APP( BlackHole02App, RendererGl )
