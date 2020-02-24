#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Rand.h"

#include "CinderARKit.h"
#include "BatchHelpers.h"


using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_PARTICLES = 50e4;
const int    FBO_WIDTH  = 2048;
const int    FBO_HEIGHT = 2048;

class PixelatedApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    void updateEnvMap();
    
    ARKit::Session mARSession;
    
private:
    BatchBallRef bBall;
    BatchAxisRef bAxis;
    
    // camera direction
    vec3 front;
    vec3 frontXZ;
    
    // shaders
    gl::GlslProgRef mShaderRender;
    gl::GlslProgRef mShaderUpdate;
    
    // particles
    // Descriptions of particle data layout.
    gl::VaoRef          mAttributes[2];
    // Buffers holding raw particle data on GPU.
    gl::VboRef          mParticleBuffer[2];
    
    std::uint32_t       mSourceIndex        = 0;
    std::uint32_t       mDestinationIndex   = 1;
    
    gl::FboRef              mFboEnv;
    
    float hasBegin = 0.0f;
    float offset = 0.0f;
    float targetOffset = 0.0f;
};


struct Particle
{
    vec3 pos;
    vec3 posOrg;
    vec3 color;
    vec3 extra;
};

void PixelatedApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Horizontal );
    
    mARSession.runConfiguration( config );
    
    // helpers
    bBall = BatchBall::create();
    bAxis = BatchAxis::create();
    
    // init particles
    console() << " Number of particles : " << NUM_PARTICLES << endl;
    vector<Particle> particles;
    particles.assign( NUM_PARTICLES, Particle() );
    
    float xRange = 0.25f;
    float yRange = 0.5f;
    float zRange = 1.0f;
    
    
    for( int i =0; i<particles.size(); i++) {
        float x;
        
        float t = randFloat();
        if(t < 0.9) {
            x = randFloat(xRange * 0.75, xRange);
            if(randFloat() > .5) {
                x *= -1.0;
            }
        } else {
            x = randFloat(-xRange * .75, xRange * .75);
        }
        
        
        float y = randFloat(-yRange, yRange);
        float z = randFloat(0, -zRange);
        
        auto &p = particles.at( i );
        
        p.pos = vec3(x, y, z);
        p.posOrg = vec3(x, y, z - zRange * 0.5);
        p.color = vec3(0, 0, 0);
        p.extra = vec3(0, randFloat(), randFloat());
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
        gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
        gl::vertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, posOrg) );
        gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color) );
        gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, extra) );
    }
    
    // init shaders
    mShaderRender = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vert" ) ).fragment( loadAsset("render.frag"))
       .attribLocation( "ciPosition", 0 )
       .attribLocation( "iPositionOrg", 1 )
       .attribLocation( "iColor", 2 )
       .attribLocation( "iExtra", 3 )
    );
    
    mShaderUpdate = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "update.vert" ) ).fragment( loadAsset( "no_op.frag" ) )
        .feedbackFormat( GL_INTERLEAVED_ATTRIBS )
        .feedbackVaryings( { "position", "positionOrg", "color", "extra"} )
        .attribLocation( "iPosition", 0 )
        .attribLocation( "iPositionOrg", 1 )
        .attribLocation( "iColor", 2 )
        .attribLocation( "iExtra", 3 )
    );
    
    // frame buffer
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormatEnv.colorTexture() );
}

void PixelatedApp::touchesBegan( TouchEvent event )
{
    hasBegin = 1.0;
    targetOffset = 1.0f;
}

void PixelatedApp::updateEnvMap() {
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedMatrices matScp;
    
    mARSession.drawRGBCaptureTexture(getWindowBounds());
}

void PixelatedApp::update()
{
    offset += (targetOffset - offset) * 0.1f;
    updateEnvMap();
    front = AlfridUtils::getLookDir(mARSession.getViewMatrix());
    frontXZ = front * vec3(1.0, 0.0, 1.0);
    frontXZ = glm::normalize(frontXZ);
    vec3 _front = vec3(0.0, 0.0, -1.0);
    
    vec3 axis = glm::cross(_front, frontXZ);
    float theta = acos(glm::dot(_front, frontXZ));
    mat4 mtxAlign = glm::rotate(theta, axis);
    
    
    gl::ScopedMatrices matScp;
    mat4 shadowMatrix = mARSession.getProjectionMatrix() * mARSession.getViewMatrix();
    
    // Update particles on the GPU
    gl::ScopedGlslProg prog( mShaderUpdate );
    gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );    // turn off fragment stage
    
    mShaderUpdate->uniform("uMatrix", shadowMatrix);
    mShaderUpdate->uniform("uAlignMatrix", mtxAlign);
    mShaderUpdate->uniform("uCameraPos", mARSession.getCameraPosition());
    mShaderUpdate->uniform("uLookDir", front);
    mShaderUpdate->uniform("uHasBegin", hasBegin);
    
    gl::ScopedTextureBind texScopeEnv( mFboEnv->getColorTexture(), (uint8_t) 0 );
    mShaderUpdate->uniform( "uEnvMap", 0 );
    

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
}

void PixelatedApp::draw()
{
    
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::disableDepthWrite();
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::enableDepth();
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    
    // render particles
    gl::ScopedGlslProg prog( mShaderRender );
    mShaderRender->uniform("uViewport", vec2(getWindowSize()));
    mShaderRender->uniform("uOffset", offset);
    
    gl::ScopedVao vao( mAttributes[mSourceIndex] );
    gl::context()->setDefaultShaderVars();
    gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
    
    
//    gl::setMatricesWindow( toPixels( getWindowSize() ) );
//    int ss = 128 * 2;
//    gl::draw( mFboEnv->getColorTexture(), Rectf( 0, 0, ss, ss/getWindowAspectRatio() ) );
}

CINDER_APP( PixelatedApp, RendererGl )
