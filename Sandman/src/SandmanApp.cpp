#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "Helpers.h"
#include "OrbitalControl.h"

#include "KinectAzure.h"


using namespace ci;
using namespace ci::app;
using namespace std;
using namespace alfrid;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class SandmanApp : public App {
public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	void resize() override;

private:
	void initParticles();
	void updateShadowMap();

	// camera
	CameraPersp			mCamera;
	CameraPersp         mCamLight;
	OrbitalControl*     mOrbControl;


	// shaders
	gl::GlslProgRef         mShaderUpdate;
	gl::GlslProgRef         mShaderRender;

	// particles
	gl::VaoRef              mAttributes[2];
	gl::VboRef              mParticleBuffer[2];

	std::uint32_t           mSourceIndex = 0;
	std::uint32_t           mDestinationIndex = 1;

	gl::FboRef              mFbo;
	gl::Texture2dRef        mShadowMapTex;
	gl::Texture2dRef        mParticleTex;

	vec3                    mLightPos;
	float                   mSeed = randFloat(1000.0f);

	// controls
    params::InterfaceGlRef mParams;
    float fps = 0.0f;
};


const int    NUM_PARTICLES = 30e5;
const int    FBO_WIDTH = 2048;
const int    FBO_HEIGHT = 2048;

struct Particle
{
	vec3    pos;
	vec3    vel;
	vec3    posOrg;
	vec3    random;
	float   life;
};

void prepareSettings( SandmanApp::Settings *settings) {
    settings->setHighDensityDisplayEnabled();
    settings->setMultiTouchEnabled( false );
    settings->setTitle("SandmanApp");
}


void SandmanApp::setup()
{
	CI_LOG_D(NUM_PARTICLES / float(1024 * 1024));
    setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	auto display = Display::getMainDisplay();
	setWindowPos( (display->getWidth() - WINDOW_WIDTH) * 0.5, (display->getHeight() - WINDOW_HEIGHT) * 0.5);
	setWindowPos( 0, 50);
	setFrameRate(60.0f);
	gl::enableDepth();
	gl::enable(GL_POINT_SPRITE_ARB); // or use: glEnable
	gl::enable(GL_VERTEX_PROGRAM_POINT_SIZE);   // or use: glEnable

	// camera
	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 75.0f, 0.1f, 50.0f);
	mCamera.lookAt(vec3(-2, 2, 5), vec3(0));
	mOrbControl = new OrbitalControl(&mCamera, getWindow());
	mOrbControl->rx->setValue(0.5);
    mOrbControl->ry->setValue(0.5);

	// textures
	mParticleTex = gl::Texture2d::create(loadImage(loadAsset("particle.png")));

	// controls
    mParams = params::InterfaceGl::create("Controls", vec2(200, 200));
    mParams->addParam("FPS", &fps);

	initParticles();
}

void SandmanApp::initParticles() {
	vector<Particle> particles;
	particles.assign(NUM_PARTICLES, Particle());

	for (int i = 0; i < particles.size(); i++) {
		vec3 pos = randVec3() * randFloat();
		auto& p = particles.at(i);

		p.pos = pos;
		p.posOrg = pos;
		p.life = Rand::randFloat(0.01f, 1.0f);
		p.random = vec3(randFloat(), randFloat(), randFloat());
	}

	mParticleBuffer[mSourceIndex] = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
	mParticleBuffer[mDestinationIndex] = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW);

	//gl::Vbo::create(GL)

	for (int i = 0; i < 2; ++i)
	{    // Describe the particle layout for OpenGL.
		mAttributes[i] = gl::Vao::create();
		gl::ScopedVao vao(mAttributes[i]);

		// Define attributes as offsets into the bound particle buffer
		gl::ScopedBuffer buffer(mParticleBuffer[i]);
		gl::enableVertexAttribArray(0);
		gl::enableVertexAttribArray(1);
		gl::enableVertexAttribArray(2);
		gl::enableVertexAttribArray(3);
		gl::enableVertexAttribArray(4);
		gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos));
		gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel));
		gl::vertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, posOrg));
		gl::vertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, random));
		gl::vertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, life));

	}


	mShaderRender = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("render.vert")).fragment(loadAsset("render.frag"))
		.attribLocation("ciPosition", 0)
		.attribLocation("iPositionOrg", 2)
		.attribLocation("iRandom", 3)
		.attribLocation("iLife", 4)
	);
	mShaderUpdate = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("update.vert"))
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
		.feedbackVaryings({ "position", "velocity", "positionOrg", "random", "life" })
		.attribLocation("iPosition", 0)
		.attribLocation("iVelocity", 1)
		.attribLocation("iPositionOrg", 2)
		.attribLocation("iRandom", 3)
		.attribLocation("iLife", 4)
	);


	// shadow mapping
	mLightPos = vec3(.1, 10.0, 0.1);
	gl::Texture2d::Format depthFormat;
	depthFormat.setInternalFormat(GL_DEPTH_COMPONENT32F);
	depthFormat.setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
	depthFormat.setMagFilter(GL_LINEAR);
	depthFormat.setMinFilter(GL_LINEAR);
	depthFormat.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	depthFormat.setCompareFunc(GL_LEQUAL);
	mShadowMapTex = gl::Texture2d::create(FBO_WIDTH, FBO_HEIGHT, depthFormat);

	gl::Fbo::Format fboFormat;
	fboFormat.attachment(GL_DEPTH_ATTACHMENT, mShadowMapTex);
	mFbo = gl::Fbo::create(FBO_WIDTH, FBO_HEIGHT, fboFormat);

	// Set up camera from the light's viewpoint
	mCamLight.setPerspective(100.0f, mFbo->getAspectRatio(), 0.5f, 20.0f);
	mCamLight.lookAt(mLightPos, vec3(0));
}

void SandmanApp::keyDown( KeyEvent event )
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

void SandmanApp::updateShadowMap() {
	gl::ScopedFramebuffer fbo(mFbo);
	gl::ScopedViewport viewport(vec2(0.0f), mFbo->getSize());

	gl::clear(Color(0, 0, 0));
	gl::setMatrices(mCamLight);
	gl::ScopedGlslProg prog(mShaderRender);

	mShaderRender->uniform("uViewport", vec2(getWindowSize()));
	gl::ScopedVao vao(mAttributes[mSourceIndex]);
	gl::context()->setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

void SandmanApp::update()
{
	fps = getAverageFps();

	// particle simulation
	gl::ScopedGlslProg prog(mShaderUpdate);
	gl::ScopedState rasterizer(GL_RASTERIZER_DISCARD, true);    // turn off fragment stage
	mShaderUpdate->uniform("uTime", float(getElapsedSeconds()) + mSeed);

	gl::ScopedVao source(mAttributes[mSourceIndex]);
	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex]);
	gl::beginTransformFeedback(GL_POINTS);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::endTransformFeedback();

	std::swap(mSourceIndex, mDestinationIndex);
}

void SandmanApp::draw()
{
	updateShadowMap();
	gl::clear();

    gl::setMatrices(mCamera);
	alfrid::helpers::drawAxis();
	alfrid::helpers::drawDotPlanes();
	alfrid::helpers::drawBall(mLightPos, vec3(.2), vec3(1, 1, .5));

	mat4 shadowMatrix = mCamLight.getProjectionMatrix() * mCamLight.getViewMatrix();
	gl::ScopedGlslProg prog(mShaderRender);
	mShaderRender->uniform("uViewport", vec2(getWindowSize()));
	mShaderRender->uniform("uShadowMatrix", shadowMatrix);

	gl::ScopedTextureBind texScope(mShadowMapTex, (uint8_t)0);
	mShaderRender->uniform("uShadowMap", 0);
	gl::ScopedTextureBind texParticle(mParticleTex, (uint8_t)1);
	mShaderRender->uniform("uParticleMap", 1);

	gl::ScopedVao vao(mAttributes[mSourceIndex]);
	gl::context()->setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	mParams->draw();
}

void SandmanApp::resize() {
	mCamera.setPerspective(75, getWindowAspectRatio(), .1f, 100.0f);
}

CINDER_APP( SandmanApp, RendererGl( RendererGl::Options().msaa( 4 ) ), prepareSettings )
