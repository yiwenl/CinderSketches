#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"
#include "CinderARKit.h"
#include "BatchHelpers.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int    FBO_WIDTH = 2048;
const int    FBO_HEIGHT = 2048;
const int    num = 5;


struct InstanceData {
    vec3 pos;
    vec3 extra;
};

class TesseractApp : public App {
  public:
	void setup() override;
	void touchesBegan( TouchEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    void updateEnvMap();
    
    ARKit::Session mARSession;
    
    gl::FboRef          mFboEnv;
    TriMeshRef          mMesh;
    
    gl::BatchRef        mTesseract;
    gl::GlslProgRef     mShader;
    
    gl::VboMeshRef      mesh;
    
    mat4                mtxScale;
    vec3                mPos;
    
    
    BatchBallRef        bBall;
    EaseNumberRef       mOffset;
};

void TesseractApp::setup()
{
    auto config = ARKit::SessionConfiguration()
                        .trackingType( ARKit::TrackingType::WorldTracking )
                        .planeDetection( ARKit::PlaneDetection::Both );
    
    mARSession.runConfiguration( config );
    gl::enableDepth();
    
    
    ObjLoader loader( loadAsset("side.obj") );
    mMesh = TriMesh::create( loader );
    mesh = gl::VboMesh::create(loader);
    

    if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
        mMesh->recalculateNormals();
    
    vector<InstanceData> instanceData;
    
    for(int i=0; i<num; i++) {
        InstanceData data0;
        data0.pos = vec3(i, i, i);
        data0.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data0);
        
        InstanceData data1;
        data1.pos = vec3(-i, i, i);
        data1.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data1);
        
        InstanceData data2;
        data2.pos = vec3(i, -i, i);
        data2.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data2);
        
        InstanceData data3;
        data3.pos = vec3(-i, -i, i);
        data3.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data3);
        
        InstanceData data4;
        data4.pos = vec3(i, -i, -i);
        data4.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data4);
        
        InstanceData data5;
        data5.pos = vec3(-i, -i, -i);
        data5.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data5);
        
        InstanceData data6;
        data6.pos = vec3(i, i, -i);
        data6.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data6);
        
        InstanceData data7;
        data7.pos = vec3(-i, i, -i);
        data7.extra = vec3(float(i) / float(num), randFloat(), randFloat());
        instanceData.push_back(data7);
    }
    
    gl::VboRef mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, instanceData.size() * sizeof(InstanceData), instanceData.data(), GL_STATIC_DRAW );
    geom::BufferLayout instanceDataLayout;
    instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, sizeof( InstanceData ), offsetof( InstanceData, pos ), 1 /* per instance*/ );
    instanceDataLayout.append( geom::Attrib::CUSTOM_1, 3, sizeof( InstanceData ), offsetof( InstanceData, extra ), 1 /* per instance*/ );
    mesh->appendVbo( instanceDataLayout, mInstanceDataVbo );
    

    mShader = gl::GlslProg::create( loadAsset( "tess.vert" ), loadAsset( "tess.frag" ) );
    mTesseract = gl::Batch::create( mesh, mShader, {
        { geom::Attrib::CUSTOM_0, "aPosOffset" },
        { geom::Attrib::CUSTOM_1, "aExtra" }
    } );
    
    // frame buffer
    gl::Fbo::Format fboFormatEnv;
    mFboEnv = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormatEnv.colorTexture() );
    
    float s = 0.5f;
    mtxScale = glm::scale(mtxScale, vec3(s));
    
    bBall = BatchBall::create();
    
    
    mOffset = EaseNumber::create(0.0f);
    mOffset->easing = 0.03f;
}

void TesseractApp::touchesBegan( TouchEvent event )
{
    if(mOffset->getTargetValue() < 0.5) {
        mOffset->setValue(1.0f);
    } else {
        mOffset->setValue(0.0f);
    }
    
}

void TesseractApp::updateEnvMap() {
    gl::ScopedFramebuffer fbo( mFboEnv );
    gl::ScopedViewport viewport( vec2( 0.0f ), mFboEnv->getSize() );
    gl::clear( Color( 0, 0, 0 ) );

    gl::ScopedMatrices matScp;

    mARSession.drawRGBCaptureTexture(getWindowBounds());
}

void TesseractApp::update()
{
    updateEnvMap();
    if(mOffset->getTargetValue() < 0.5) {
        mPos = vec3(mARSession.getCameraPosition());
    }
    
    mOffset->update();
}


void TesseractApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
//    gl::disableDepthRead();
//    mARSession.drawRGBCaptureTexture( getWindowBounds() );
    gl::enableDepth();
    
    gl::ScopedMatrices matScp;
    gl::setViewMatrix( mARSession.getViewMatrix() );
    gl::setProjectionMatrix( mARSession.getProjectionMatrix() );
    
    gl::ScopedTextureBind texScopeEnv( mFboEnv->getColorTexture(), (uint8_t) 0 );
    mShader->uniform( "uEnvMap", 0 );
    mShader->uniform( "uMtxScale", mtxScale);
    mShader->uniform( "uOffset", mOffset->getValue());
    mShader->uniform( "uPos", mPos);
    mShader->uniform( "uTime", (float)getElapsedSeconds());
    
    mat4 m;
    for(int i=0; i<4; i++) {
        m = glm::mat4();
        float a = M_PI * 0.5f * i;
        m = glm::rotate(m, a, vec3(0, 1, 0));
        gl::setModelMatrix(m);
        mTesseract->drawInstanced(num * 8);
    }
    
   
    for(int i=0; i<2; i++) {
        m = glm::mat4();
        float a = M_PI * i + M_PI * 0.5f;
        m = glm::rotate(m, a, vec3(1, 0, 0));
        gl::setModelMatrix(m);
        mTesseract->drawInstanced(num * 8);
    }
    
}

CINDER_APP( TesseractApp, RendererGl )
