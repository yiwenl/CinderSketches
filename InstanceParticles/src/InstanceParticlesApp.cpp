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


const int NUM_PARTICLES = 10e2;

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

	/*gl::VaoRef			mVao0;
	gl::VaoRef			mVao1;
	gl::VboRef			mVboPos0;
	gl::VboRef			mVboPos1;*/

	gl::GlslProgRef		mShaderUpdate;
	gl::GlslProgRef		mShaderRender;

	gl::BatchRef		mBatch;

	vector<gl::VaoRef> mVaos;
	vector<vector<gl::VboRef>> mVbos;

	int pivot = 0;

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
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 50.0f);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
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

	mShaderUpdate->uniform("uTime", (float)getElapsedSeconds());
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
	gl::clear();

    gl::setMatrices(mCamera);
	alfrid::helpers::drawAxis();
	alfrid::helpers::drawDotPlanes();


	gl::ScopedGlslProg shader(mShaderRender);
	gl::ScopedVao vao(mVaos.at(pivot));
	gl::context()->setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	mParams->draw();
}

void InstanceParticlesApp::resize() {
	mCamera.setPerspective(75, getWindowAspectRatio(), .1f, 100.0f);
}

CINDER_APP( InstanceParticlesApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
