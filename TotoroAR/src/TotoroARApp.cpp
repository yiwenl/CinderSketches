#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/ObjLoader.h"

#include "CinderARKit.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TotoroARApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void loadObj( const DataSourceRef &dataSource );
    
    ARKit::Session     mARSession;
    
    gl::Texture2dRef   mTexture;
        
    TriMeshRef          mMesh;
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    gl::TextureRef      mTextureColor;
    gl::TextureRef      mTextureNormal;
    gl::TextureRef      mTextureAO;
};

void TotoroARApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .imageTrackingEnabled( true )
                        .trackingType( ARKit::TrackingType::WorldTracking );
    mARSession.runConfiguration( config );
    
    // This texture is just used for visualisation
    mTexture = gl::Texture2d::create( loadImage( loadAsset( "totoro.jpg" )));
    mTextureColor = gl::Texture2d::create( loadImage( loadAsset( "color.jpg" )));
    mTextureNormal = gl::Texture2d::create( loadImage( loadAsset( "normal.jpg" )));
    mTextureAO = gl::Texture2d::create( loadImage( loadAsset( "ao.jpg" )));
    
    
    mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
    mGlsl->uniform( "uColorMap", 0 );
    mGlsl->uniform( "uNormalMap", 1 );
    mGlsl->uniform( "uAOMap", 2 );
    
    
    loadObj( loadResource( RES_TOTORO_OBJ ) );
}

void TotoroARApp::loadObj( const DataSourceRef &dataSource )
{
    ObjLoader loader( dataSource );
    mMesh = TriMesh::create( loader );

    if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
        mMesh->recalculateNormals();

    mBatch = gl::Batch::create( *mMesh, mGlsl );
    
}

void TotoroARApp::mouseDown( MouseEvent event )
{
}

void TotoroARApp::update()
{
}

void TotoroARApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::disableDepthWrite();
    gl::disableDepthRead();
        
    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    
    gl::enableDepthWrite();
    gl::enableDepthRead();

    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );

//    gl::ScopedGlslProg glslProg( gl::getStockShader( gl::ShaderDef().color().texture() ));
//    gl::ScopedTextureBind texScp( mTexture );
//    gl::ScopedColor colScp;
//    gl::color( 1.0f, 1.0f, 1.0f );
    


    for (const auto& a : mARSession.getImageAnchors())
    {
       gl::ScopedMatrices matScp;
       gl::setModelMatrix( a.mTransform );
       
//       gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
//
//       gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
//       const float xRad = a.mPhysicalSize.x * 0.5f;
//       const float yRad = a.mPhysicalSize.y * 0.5f;
//
//       // Overlay tracked image
//       for (int i = 0; i < 10; ++i)
//       {
//           if (i < (int)(getElapsedSeconds() * 5.0f) % 10)
//           {
//               gl::translate( 0.0f, 0.0f, -0.01f );
//               gl::drawSolidRect( Rectf( -xRad,-yRad, xRad, yRad ));
//           }
//       }
        
        
        gl::ScopedGlslProg prog( mGlsl );
        
        mTextureColor->bind(0);
        mTextureNormal->bind(1);
        mTextureAO->bind(2);
        
        mBatch->draw();

       /* a.mImageName will allow to decide what AR content to show, attached to this anchor*/
    }
        
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = 128 * 2;
    float ratio = (float)mTexture->getWidth() / (float)mTexture->getHeight();
    gl::draw( mTexture, Rectf( 0, 0, s, s/ratio ) );
}

CINDER_APP( TotoroARApp, RendererGl )
