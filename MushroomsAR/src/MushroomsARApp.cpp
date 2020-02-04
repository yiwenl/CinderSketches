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
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session     mARSession;
    
    gl::Texture2dRef   mTexture;
    
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
    
private:
    void updateShadowMap();
    void initParticles();
    
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
    
    
    mShaderColor = gl::GlslProg::create( loadAsset( "basic.vert" ), loadAsset( "color.frag" ) );
    float ratio = (float)mTexture->getWidth() / (float)mTexture->getHeight();
    console() << "Image Ratio : " << ratio << endl;
    // scale : 0.2
    auto box = gl::VboMesh::create( geom::Cube().size(1.0f, 1.0f, 0.1f) );
    mBox = gl::Batch::create(box, mShaderColor);
    
    
    initParticles();
}

void MushroomsARApp::initParticles() {
    // init particles
    console() << "Number of particles : " << Config::getInstance().NUM_PARTICLES << endl;
    vector<Particle> particles;
    particles.assign( Config::getInstance().NUM_PARTICLES, Particle() );
    float ratio = (float)mTexture->getWidth() / (float)mTexture->getHeight();
    float rangeX = 0.1;
    float rangeY = 0.294/2.0;
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

void MushroomsARApp::mouseDown( MouseEvent event )
{
}

void MushroomsARApp::update()
{
}

void MushroomsARApp::draw()
{
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

    gl::ScopedGlslProg glslProg( mShaderColor );
    mShaderColor->uniform("uColor", vec3(1.0));
       
       
    for (const auto& a : mARSession.getImageAnchors())
    {
        gl::ScopedMatrices matScp;
        gl::setModelMatrix( a.mTransform );
        mShaderColor->uniform("uSize", a.mPhysicalSize);

        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
        
//        mBox->draw();
        
        gl::ScopedGlslProg progRender( mShaderRender );
        mShaderRender->uniform("uViewport", vec2(getWindowSize()));
        
        gl::ScopedTextureBind texMap( mTexture, (uint8_t) 0 );
        mShaderRender->uniform( "uColorMap", 0 );
        
        gl::ScopedVao vao( mAttributes[mSourceIndex] );
        gl::context()->setDefaultShaderVars();
        gl::drawArrays( GL_POINTS, 0, Config::getInstance().NUM_PARTICLES );
        

    /* a.mImageName will allow to decide what AR content to show, attached to this anchor*/
    }
    
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = 128 * 2;
    float ratio = (float)mTexture->getHeight() / (float)mTexture->getWidth();
    gl::draw( mTexture, Rectf( 0, 0, s, s * ratio ) );
}

CINDER_APP( MushroomsARApp, RendererGl )
