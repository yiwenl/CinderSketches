#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderARKit.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const float IMAGE_RATIO = 1080.0f/1588.0f;

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
    
};

void MushroomsARApp::setup()
{
    // Tracked images are stored in Images.xcassets / AR Resources
    // You must set the image size of the asset in real world units.
    // The image in this demo is set to 12cm x 12cm which is roughly the size of the
    // image when you open it in preview.
    
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
}

void MushroomsARApp::mouseDown( MouseEvent event )
{
}

void MushroomsARApp::update()
{
}

void MushroomsARApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
    mARSession.drawRGBCaptureTexture( getWindowBounds() );

    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );

    gl::ScopedGlslProg glslProg( mShaderColor );
    mShaderColor->uniform("uColor", vec3(1.0));
       
       
    for (const auto& a : mARSession.getImageAnchors())
    {
        gl::ScopedMatrices matScp;
        gl::setModelMatrix( a.mTransform );
        console() << "Size : " << a.mPhysicalSize << endl;
        mShaderColor->uniform("uSize", a.mPhysicalSize);
//
//        gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
//
        gl::rotate( (float)M_PI * 0.5f, vec3(1,0,0) ); // Make it parallel with the ground
//        const float xRad = a.mPhysicalSize.x * 0.5f;
//        const float yRad = a.mPhysicalSize.y * 0.5f;
//
//        // Overlay tracked image
//        for (int i = 0; i < 10; ++i)
//        {
//           if (i < (int)(getElapsedSeconds() * 5.0f) % 10)
//           {
//               gl::translate( 0.0f, 0.0f, -0.01f );
//               gl::drawSolidRect( Rectf( -xRad,-yRad, xRad, yRad ));
//           }
//        }
        
        mBox->draw();
        

    /* a.mImageName will allow to decide what AR content to show, attached to this anchor*/
    }
    
    gl::setMatricesWindow( toPixels( getWindowSize() ) );
    int s = 128 * 2;
    gl::draw( mTexture, Rectf( 0, 0, s, s/IMAGE_RATIO ) );
}

CINDER_APP( MushroomsARApp, RendererGl )
