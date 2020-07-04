#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
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

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;


const int NUM_PARTICLES = 5e5;
const float FOV = 45.0f;
const float FAR = 30.0f;
const float NEAR = 0.1f;

class InstanceParticlesApp : public App {
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

	gl::GlslProgRef		mShaderUpdate;
	gl::GlslProgRef		mShaderRender;
	gl::GlslProgRef		mShaderCube;
	gl::GlslProgRef		mShaderAO;

	gl::BatchRef		mBatch;
	gl::BatchRef		mBatchAO;

	gl::Texture2dRef	mDepthTex;
	gl::FboRef			mFbo;
	gl::FboRef			mFboSSAO;
	

	vector<gl::VaoRef> mVaos;
	vector<vector<gl::VboRef>> mVbos;

	int pivot = 0;
	float seed = randFloat(5000.0);

	// controls
    params::InterfaceGlRef mParams;
    float fps = 0.0f;
};


void prepareSettings( InstanceParticlesApp::Settings *settings) {
    settings->setHighDensityDisplayEnabled();
    settings->setMultiTouchEnabled( false );
    settings->setTitle("InstanceParticlesApp");
}


void InstanceParticlesApp::setup()
{
	CI_LOG_D("Number of particles : " << NUM_PARTICLES);
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setFrameRate(60.0f);
	gl::enableDepth();
	gl::enable(GL_POINT_SPRITE_ARB);
	gl::enable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), FOV, NEAR, FAR);
	mCamera.lookAt(vec3(-2, 2, 5) * 2.0f, vec3(0));
	mOrbControl = new OrbitalControl(&mCamera, getWindow());
	mOrbControl->rx->setValue(0.5);
    mOrbControl->ry->setValue(0.5);


	// shaders
	mShaderRender = gl::GlslProg::create(gl::GlslProg::Format()
						.vertex( loadAsset("render.vert"))
						.fragment( loadAsset("render.frag"))
						.attribLocation("ciPosition", 0)
						.attribLocation("aExtra", 2)
					);

	mShaderUpdate = gl::GlslProg::create(gl::GlslProg::Format()
		.vertex(loadAsset("update.vert"))
		.feedbackFormat(GL_SEPARATE_ATTRIBS)
		.feedbackVaryings({ "position", "velocity", "extra" })
		.attribLocation("iPosition", 0)
		.attribLocation("iVelocity", 1)
		.attribLocation("iExtra", 2)
	);


					

	// init points
	vector<vec3> points;
	vector<vec3> vel;
	vector<vec3> extras;
	for (int i = 0; i < NUM_PARTICLES; i++) {
		vec3 v = randVec3() * randFloat();
		points.push_back(v);
		vel.push_back(vec3(0));
		extras.push_back(randVec3() * 0.5f + 0.5f);
	}

	for (int i = 0; i < 2; i++) {
		gl::VaoRef vao = gl::Vao::create();

		gl::VboRef vboPos = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_PARTICLES, points.data(), GL_STATIC_DRAW);
		gl::VboRef vboVel = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_PARTICLES, vel.data(), GL_STATIC_DRAW);
		gl::VboRef vboExtra = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_PARTICLES, extras.data(), GL_STATIC_DRAW);

		vector<gl::VboRef> vbos;
		vbos.push_back(vboPos);
		vbos.push_back(vboVel);
		vbos.push_back(vboExtra);

		mVbos.push_back(vbos);
		mVaos.push_back(vao);

		gl::ScopedVao _vao(vao);
		gl::ScopedBuffer vPos(vboPos);
		gl::enableVertexAttribArray(0);
		gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
		gl::ScopedBuffer vVel(vboVel);
		gl::enableVertexAttribArray(1);
		gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
		gl::ScopedBuffer vExtra(vboExtra);
		gl::enableVertexAttribArray(2);
		gl::vertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}


	gl::VboMeshRef mesh = gl::VboMesh::create(geom::Cube());

	geom::BufferLayout instancedLayout0;
	instancedLayout0.append(geom::Attrib::CUSTOM_0, 3, 0, 0, 1);
	geom::BufferLayout instancedLayout1;
	instancedLayout1.append(geom::Attrib::CUSTOM_1, 3, 0, 0, 1);
	geom::BufferLayout instancedLayoutVel0;
	instancedLayoutVel0.append(geom::Attrib::CUSTOM_2, 3, 0, 0, 1);
	geom::BufferLayout instancedLayoutVel1;
	instancedLayoutVel1.append(geom::Attrib::CUSTOM_3, 3, 0, 0, 1);
	geom::BufferLayout instancedLayoutExtra;
	instancedLayoutExtra.append(geom::Attrib::CUSTOM_4, 3, 0, 0, 1);

	mesh->appendVbo(instancedLayout0, mVbos.at(0).at(0));
	mesh->appendVbo(instancedLayout1, mVbos.at(1).at(0));
	mesh->appendVbo(instancedLayoutVel0, mVbos.at(0).at(1));
	mesh->appendVbo(instancedLayoutVel1, mVbos.at(1).at(1));
	mesh->appendVbo(instancedLayoutExtra, mVbos.at(0).at(2));

	mShaderCube = gl::GlslProg::create(loadAsset("cubes.vert"), loadAsset("cubes.frag"));
	mBatch = gl::Batch::create(mesh, mShaderCube, { 
		{geom::Attrib::CUSTOM_0, "aPosOffset0"}, 
		{geom::Attrib::CUSTOM_1, "aPosOffset1"},
		{geom::Attrib::CUSTOM_2, "aVel0"},
		{geom::Attrib::CUSTOM_3, "aVel1"},
		{geom::Attrib::CUSTOM_4, "aExtra"},
		});


	// post
	mShaderAO = gl::GlslProg::create(loadAsset("pass.vert"), loadAsset("ao.frag"));

	int FBO_SIZE = 2048;
	gl::Texture2d::Format depthFormat;
	depthFormat.setInternalFormat(GL_DEPTH_COMPONENT32F);
	mDepthTex = gl::Texture2d::create(FBO_SIZE, FBO_SIZE, depthFormat);

	gl::Fbo::Format format;
	int numSample = 8;
	format.setSamples(numSample);
	format.setCoverageSamples(numSample);
	format.attachment(GL_DEPTH_ATTACHMENT, mDepthTex);
	mFbo = gl::Fbo::create(FBO_SIZE, FBO_SIZE, format);

	auto plane = gl::VboMesh::create(geom::Plane().normal(vec3(0, 0, 1)));
	mBatchAO = gl::Batch::create(plane, mShaderAO);


	// controls
    mParams = params::InterfaceGl::create("Controls", vec2(200, 200));
    mParams->addParam("FPS", &fps);
}

void InstanceParticlesApp::keyDown( KeyEvent event )
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

void InstanceParticlesApp::update()
{
	fps = getAverageFps();


	gl::ScopedGlslProg shader(mShaderUpdate);
	gl::ScopedState rasterizer(GL_RASTERIZER_DISCARD, true);    // turn off fragment stage

	mShaderUpdate->uniform("uTime", (float)getElapsedSeconds() + seed);
	gl::ScopedVao vaoSource(mVaos.at(pivot));

	auto vbos = mVbos.at(1 - pivot);
	for (int i = 0; i < vbos.size(); i++) {
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, i, vbos.at(i));
	}
	gl::beginTransformFeedback(GL_POINTS);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
	gl::endTransformFeedback();

	// ping pong
	pivot = 1 - pivot;
}

void InstanceParticlesApp::draw()
{
	float g = 0.95;
	gl::clear(Color(g, g, g));

	{
		gl::ScopedFramebuffer fbo(mFbo);
		gl::clear(Color(g, g, g));
		gl::ScopedViewport viewport(vec2(0), mFbo->getSize());
		gl::ScopedGlslProg shader(mShaderCube);
		gl::setMatrices(mCamera);
		mShaderCube->uniform("uPivot", (float)pivot);
		mBatch->drawInstanced(NUM_PARTICLES);
	}

	gl::viewport(getWindowSize());
	gl::setMatricesWindow(getWindowSize());
	//gl::draw(mFbo->getColorTexture(), Rectf(0, 0, getWindowWidth(), getWindowHeight()));	

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

	//mParams->draw();
}

void InstanceParticlesApp::resize() {
	mCamera.setPerspective(FOV, getWindowAspectRatio(), NEAR, FAR);
}

CINDER_APP( InstanceParticlesApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
