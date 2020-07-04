#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "Helpers.h"
#include "OrbitalControl.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const int numPoints = 10000;
const float RANGE = 100.0f;
const float FOV = 45.0f;
const float FAR = 100.0f;
const float NEAR = 0.1f;

class AmbientApp : public App {
public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	void resize() override;

private:
	// camera
	CameraPersp			mCamera;
	OrbitalControl*     mOrbControl;


	gl::VboRef mVboPos;
	gl::VboRef mVboSize;
	vector<vec3> mPoints;

	gl::FboRef	mFbo;
	gl::FboRef  mFboSSAO;
	gl::Texture2dRef mDepthTex;

	gl::GlslProgRef mShaderCube;
	gl::GlslProgRef mShaderAO;
	gl::BatchRef mBatchAO;
	gl::BatchRef mBatchCube;

	// controls
    params::InterfaceGlRef mParams;
    float fps = 0.0f;

	bool hasSaved = false;
};


void prepareSettings( AmbientApp::Settings *settings) {
    settings->setHighDensityDisplayEnabled();
    settings->setMultiTouchEnabled( false );
    settings->setTitle("AmbientApp");
}


void AmbientApp::setup()
{
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setWindowPos(0, 30);
	setFrameRate(60.0f);
	gl::enableDepth();

	// Point size
	gl::enable(GL_POINT_SPRITE_ARB);
	gl::enable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), FOV, NEAR, FAR);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
	mOrbControl = new OrbitalControl(&mCamera, getWindow());
	float r = 0.1;
	mOrbControl->rx->limit(-r, r);
	mOrbControl->ry->limit(-r, r);
	//mOrbControl->rx->setValue(0.5);
    //mOrbControl->ry->setValue(0.5);


	mShaderCube = gl::GlslProg::create(loadAsset("cubes.vert"), loadAsset("cubes.frag"));
	mShaderAO = gl::GlslProg::create(loadAsset("pass.vert"), loadAsset("ao.frag"));
	

	vector<vec3> mSizes;
	for (int i = -numPoints; i <= numPoints; i++) {
		float r = randFloat(1.5, 10.0);
		float a = randFloat(M_PI * 2.0);
		float x = cos(a) * r;
		float y = sin(a) * r;
		float z = randFloat(-RANGE, RANGE);

		mPoints.push_back(vec3(x, y, z));
		mSizes.push_back(randVec3() * 0.5f + 0.5f);
	}

	
	mVboPos = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints.size() * sizeof(vec3), mPoints.data(), GL_STATIC_DRAW);
	mVboSize = gl::Vbo::create(GL_ARRAY_BUFFER, mSizes.size() * sizeof(vec3), mSizes.data(), GL_STATIC_DRAW);

	// instancing
	gl::VboMeshRef mesh = gl::VboMesh::create(geom::Cube());

	geom::BufferLayout instanceDataLayout;
	
	instanceDataLayout.append(geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */);
	geom::BufferLayout instanceDataLayoutSize;
	instanceDataLayoutSize.append(geom::Attrib::CUSTOM_1, 3, 0, 0, 1 /* per instance */);

	mesh->appendVbo(instanceDataLayout, mVboPos);
	mesh->appendVbo(instanceDataLayoutSize, mVboSize);
	
	mBatchCube = gl::Batch::create(mesh, mShaderCube, { 
		{ geom::Attrib::CUSTOM_0, "aPosOffset" } ,
		{ geom::Attrib::CUSTOM_1, "aExtra" } 
	});

	auto plane = gl::VboMesh::create(geom::Plane().normal(vec3(0, 0, 1)));
	mBatchAO = gl::Batch::create(plane, mShaderAO);

	int FBO_SIZE = 2048;

	gl::Texture2d::Format depthFormat;
	depthFormat.setInternalFormat(GL_DEPTH_COMPONENT32F);
	mDepthTex = gl::Texture2d::create(FBO_SIZE, FBO_SIZE, depthFormat);

	gl::Fbo::Format format;
	format.setSamples(16);
	format.setCoverageSamples(16);
	format.attachment(GL_DEPTH_ATTACHMENT, mDepthTex);
	mFbo = gl::Fbo::create(FBO_SIZE, FBO_SIZE, format);

	// controls
    mParams = params::InterfaceGl::create("Controls", vec2(200, 200));
    mParams->addParam("FPS", &fps);
}

void AmbientApp::keyDown( KeyEvent event )
{
	if (event.getCode() == 27) {
		quit();
	}

	if (event.getChar() == 'f') {
		setFullScreen(!isFullScreen());
	}
	else if (event.getChar() == 's') {
		alfrid::helpers::saveImage();
	}
}

void AmbientApp::update()
{
	fps = getAverageFps();
}

void AmbientApp::draw()
{
	gl::clear();
	{
		gl::ScopedFramebuffer fbo(mFbo);
		gl::clear(Color(1, 1, 1));
		gl::ScopedViewport viewport(vec2(0), mFbo->getSize());
		gl::setMatrices(mCamera);

		gl::ScopedGlslProg shader(mShaderCube);
		mShaderCube->uniform("uTime", (float)getElapsedSeconds());
		mShaderCube->uniform("uRange", RANGE);
		mBatchCube->drawInstanced(mPoints.size());
	}

	gl::viewport(getWindowSize());
	gl::setMatricesWindow(getWindowSize());
	//gl::draw(mFbo->getColorTexture(), Rectf(0, 0, getWindowWidth(), getWindowHeight()));
	//gl::draw(mFbo->getDepthTexture(), Rectf(0, 0, getWindowWidth(), getWindowHeight()));

	{
		gl::ScopedGlslProg shader(mShaderAO);
		mShaderAO->uniform("uRatio", getWindowAspectRatio());
		mShaderAO->uniform("uTexColor", 0);
		mShaderAO->uniform("uTexDepth", 1);
		gl::ScopedTextureBind t0(mFbo->getColorTexture(), 0);
		gl::ScopedTextureBind t1(mFbo->getDepthTexture(), 1);

		mShaderAO->uniform("uNear", mCamera.getNearClip());
		mShaderAO->uniform("uFar", mCamera.getFarClip());

		mBatchAO->draw();
	
	}

	if (!hasSaved) {
		alfrid::helpers::saveImage();
		hasSaved = true;
	}
    

	mParams->draw();
}

void AmbientApp::resize() {
	mCamera.setPerspective(FOV, getWindowAspectRatio(), NEAR, FAR);
}

CINDER_APP( AmbientApp, RendererGl( RendererGl::Options().msaa( 8 ) ), prepareSettings )
