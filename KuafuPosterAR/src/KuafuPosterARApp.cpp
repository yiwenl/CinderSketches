#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "CinderARKit.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class KuafuPosterARApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
    ARKit::Session     mARSession;
    
    gl::BatchRef        mPlane;
    gl::BatchRef        mLayer;
    gl::BatchRef        mCompose;
    gl::GlslProgRef     mShaderColor;
    gl::GlslProgRef     mShaderCopy;
    gl::GlslProgRef     mShaderCompose;
    
    gl::Texture2dRef    mTexture;
    gl::Texture2dRef    mTextureBg;
    gl::Texture2dRef    mTextureSun;
    gl::Texture2dRef    mTextureGiant;
    gl::Texture2dRef    mTextureMountain;
    
    mat4                mModelMatrix;
    bool                mTouched = false;
    bool                mMtxSaved = false;
    
    float               mOffset = 0.0;
    float               mTargetOffset = 1.0;
    int                 mState = 0;
    
    gl::FboRef          mFbo;
    gl::FboRef          mFboRender;
    static const int    FBO_WIDTH = 2048, FBO_HEIGHT = 2048;
};

void KuafuPosterARApp::setup()
{
//    gl::enableDepthRead();
//    gl::enableDepthWrite();
    
    auto config = ARKit::SessionConfiguration()
                        .imageTrackingEnabled( true )
                        .trackingType( ARKit::TrackingType::WorldTracking );
    mARSession.runConfiguration( config );
    
    // init textures
    mTexture         = gl::Texture2d::create( loadImage( loadAsset( "fullColor.jpg" )));
    mTextureBg       = gl::Texture2d::create( loadImage( loadAsset( "bg.jpg" )));
    mTextureSun      = gl::Texture2d::create( loadImage( loadAsset( "sun.png" )));
    mTextureGiant    = gl::Texture2d::create( loadImage( loadAsset( "kuafu.png" )));
    mTextureMountain = gl::Texture2d::create( loadImage( loadAsset( "mountains.png" )));
    
    // init shaders
    mShaderColor    = gl::GlslProg::create( loadAsset( "basic.vert" ), loadAsset( "color.frag" ) );
    mShaderCopy     = gl::GlslProg::create( loadAsset( "basic.vert" ), loadAsset( "copy.frag" ) );
    mShaderCompose  = gl::GlslProg::create( loadAsset( "compose.vert" ), loadAsset( "compose.frag" ) );
    
    
    
    auto plane = gl::VboMesh::create( geom::Plane().size(vec2(1.0f)).normal(vec3(0.0, 0.0, 1.0)) );
    
    // init batches
    mPlane      = gl::Batch::create(plane, mShaderColor);
    mLayer      = gl::Batch::create(plane, mShaderCopy);
    mCompose    = gl::Batch::create(plane, mShaderCompose);
    
    
    // init fbo
    gl::Fbo::Format format;
    format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
    mFbo = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, format.colorTexture());
    mFboRender = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, format.colorTexture());
}

void KuafuPosterARApp::touchesBegan( TouchEvent event )
{
    mTouched = true;
    mTargetOffset = 1.0;
    mState ++;
    if(mState > 4) {
        mState = 1;
    }
    
    console() << "State : " << mState << endl;
}

void KuafuPosterARApp::update()
{
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::ScopedFramebuffer fbo( mFbo );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
    gl::clear( Color( 0, 0, 0 ) );
    
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
        
    for (const auto& a : mARSession.getImageAnchors())
    {
        gl::ScopedMatrices matScp;
        
        
        if(mTouched) {
            if(!mMtxSaved) {
                
                mModelMatrix = mat4(a.mTransform);
                console() << "Save Matrix : " << mModelMatrix << endl;
                mMtxSaved = true;
            }
            gl::setModelMatrix( mModelMatrix );
        } else {
            gl::setModelMatrix( a.mTransform );
        }
        
        gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
        
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
        gl::ScopedGlslProg progColor( mShaderColor );
        mShaderColor->uniform("uSize", a.mPhysicalSize);
        mShaderColor->uniform("uColor", vec3(1.0, 1.0, 1.0));
        mShaderColor->uniform("uScale", vec2(1.01f));
        mShaderColor->uniform("uTranslate", vec3(0.0f));
        mPlane->draw();
    }
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    gl::ScopedFramebuffer fboRender( mFboRender );
    gl::ScopedViewport viewportRender( vec2( 0.0f ), mFboRender->getSize() );
    gl::clear( ColorAf( 0.0, 0.0, 0.0, 0.0));
    
    for (const auto& a : mARSession.getImageAnchors())
    {
        gl::ScopedMatrices matScp;
        
        
        if(mTouched) {
            if(!mMtxSaved) {
                
                mModelMatrix = mat4(a.mTransform);
                console() << "Save Matrix : " << mModelMatrix << endl;
                mMtxSaved = true;
            }
            gl::setModelMatrix( mModelMatrix );
        } else {
            gl::setModelMatrix( a.mTransform );
        }
        
        
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
        
        float zOffset = 0.1f;
        float scaleOffset = 0.2f;
        float initScale = 1.1f;
        
        
        gl::ScopedBlendAlpha  blend;

        gl::ScopedGlslProg progColor( mShaderCopy );
        mShaderCopy->uniform("uSize", a.mPhysicalSize);
        mShaderCopy->uniform("uMap", 0);
        mShaderCopy->uniform("uOpacity", mOffset);
        
      
        
        mShaderCopy->uniform("uScale", vec2(1.01f));
        mShaderCopy->uniform("uTranslate", vec3(0.0f));
        mTextureBg->bind(0);
        mLayer->draw();
        
        
        
        mShaderCopy->uniform("uScale", vec2((float)(initScale + scaleOffset * 2.0)));
        mShaderCopy->uniform("uTranslate", vec3(0.0, 0.0, zOffset * 2.0));
        mTextureSun->bind(0);
        mLayer->draw();
        
        mShaderCopy->uniform("uScale", vec2((float)(initScale + scaleOffset)));
        mShaderCopy->uniform("uTranslate", vec3(0.0, 0.0, zOffset));
        mTextureGiant->bind(0);
        mLayer->draw();
        
        mShaderCopy->uniform("uScale", vec2(initScale));
        mShaderCopy->uniform("uTranslate", vec3(0.0f));
        mTextureMountain->bind(0);
        mLayer->draw();

    }
     
}



void KuafuPosterARApp::draw()
{
    
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    mOffset += (mTargetOffset - mOffset) * 0.05;
    
    
    
	gl::clear( Color( 0, 0, 0 ) );
    
    if(mState != 2 && mState != 3) {
        mARSession.drawRGBCaptureTexture( getWindowBounds() );
    }
    
    
    
    gl::ScopedMatrices matScp;
    
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    gl::ScopedGlslProg prog( mShaderCompose );
    
    mShaderCompose->uniform("uMap", 0);
    mShaderCompose->uniform("uMaskMap", 1);
    mShaderCompose->uniform("uState", (float)mState);
    
    mFboRender->getColorTexture()->bind(0);
    mFbo->getColorTexture()->bind(1);
    
    
    mCompose->draw();
    
//    gl::ScopedMatrices matScp;
//    gl::setViewMatrix( mARSession.getViewMatrix() );
//    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
//
//
    
//    int s = 128 * 2;
//    gl::draw( mFbo->getColorTexture(), Rectf( 0, 0, s, s/getWindowAspectRatio() ) );
//    gl::draw( mFboRender->getColorTexture(), Rectf( s, 0, s*2, s/getWindowAspectRatio() ) );
    
    
    
}

CINDER_APP( KuafuPosterARApp, RendererGl )
