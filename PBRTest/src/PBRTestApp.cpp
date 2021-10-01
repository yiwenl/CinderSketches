#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Log.h"
#include "cinder/ObjLoader.h"
#include "BatchHelpers.h"
#include "cinder/params/Params.h"
#include "cinder/Im"

using namespace ci;
using namespace ci::app;
using namespace std;

const string envMap = "studio";

class PBRTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    
    float metallic = 0.0;
    float roughness = 1.0;
    params::InterfaceGlRef    mParams;
    
private:
    CameraPersp mCamera;
    CameraUi    mCamUi;
    
    alfrid::BatchAxisRef        bAxis;
    alfrid::BatchGridDotsRef    bDots;
    
    gl::TextureCubeMapRef   mIrradianceMap;
    gl::TextureCubeMapRef   mRadianceMap;
    
    gl::Texture2dRef mAoMap;
    gl::Texture2dRef mBRDFMap;
    TriMeshRef        mMesh;
    
    gl::GlslProgRef mShader;
    gl::BatchRef    mBatch;
    
};

void PBRTestApp::setup()
{
    setWindowSize(1280, 720);
    setWindowPos(0, 0);
    
    gl::enableDepth();
    
    // camera
    mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 10.0f);
    mCamera.lookAt(vec3(-1.0, 1.0, 3.0), vec3(0.0));
//    mCamUi = CameraUi(&mCamera, getWindow());
    
    
    // helpers
    bAxis = alfrid::BatchAxis::create();
    bDots = alfrid::BatchGridDots::create();
    
    // maps
    auto cubeMapFormat    = gl::TextureCubeMap::Format().mipmap().internalFormat( GL_RGB16F ).minFilter( GL_LINEAR_MIPMAP_LINEAR ).magFilter( GL_LINEAR );
    string radName = envMap + "_radiance.dds";
    string irrName = envMap + "_irradiance.dds";
    mRadianceMap          = gl::TextureCubeMap::createFromDds( loadAsset( radName ), cubeMapFormat );
    mIrradianceMap        = gl::TextureCubeMap::createFromDds( loadAsset( irrName ), cubeMapFormat );
    mAoMap                = gl::Texture::create(loadImage(loadAsset("ao.png")));
    mBRDFMap              = gl::Texture::create(loadImage(loadAsset("brdfLUT.png")));
    
    // mesh
    ObjLoader loader(loadAsset("model.obj"));
    mMesh  = TriMesh::create(loader);
    
    // shader
    mShader = gl::GlslProg::create( loadAsset("shader.vert"), loadAsset("shader.frag"));
    mBatch = gl::Batch::create(*mMesh, mShader);
    
    // uniforms
    mShader->uniform("uRadianceMap", 0);
    mShader->uniform("uIrradianceMap", 1);
    mShader->uniform("uAoMap", 2);
    mShader->uniform("uBRDFMap", 3);
    
    mShader->uniform("uBaseColor", vec3(1.0));
    mShader->uniform("uLightDirection", vec3(0.5));
    mShader->uniform("uLightColor", vec3(1.0));
    mShader->uniform("uScaleDiffBaseMR", vec4(0.0));
    mShader->uniform("uScaleFGDSpec", vec4(0.0));
    mShader->uniform("uScaleIBLAmbient", vec4(1.0));
    
    //
    // Create the interface and give it a name.
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 400 ) ) );
    mParams->addParam( "Roughness", &roughness).min(0.0f).max(1.0f).step(0.01f);
    mParams->addParam( "Metallic", &metallic).min(0.0f).max(1.0f).step(0.01f);
}

void PBRTestApp::mouseDown( MouseEvent event )
{
}

void PBRTestApp::update()
{
}

void PBRTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCamera);
    bAxis->draw();
    bDots->draw();
    
    gl::ScopedGlslProg s(mShader);
    
    gl::ScopedTextureBind t0(mRadianceMap, 0);
    gl::ScopedTextureBind t1(mIrradianceMap, 1);
    gl::ScopedTextureBind t2(mAoMap, 2);
    gl::ScopedTextureBind t3(mBRDFMap, 3);
    
    mShader->uniform("uRoughness", roughness);
    mShader->uniform("uMetallic", metallic);
    mShader->uniform("uCameraPos", mCamera.getEyePoint());
    
    
    mBatch->draw();
    // Draw the interface
    mParams->draw();
}

CINDER_APP( PBRTestApp, RendererGl )
