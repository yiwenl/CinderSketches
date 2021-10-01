#include "Helpers.h"
#include "cinder/Log.h"

namespace alfrid {

	namespace helpers {

		void drawAxis(float mLineWidth)
		{
			gl::ScopedColor scp;

			gl::lineWidth(mLineWidth);
			gl::color(ColorA(1, 0, 0, 1.0f));
			gl::begin(GL_LINE_STRIP);
			gl::vertex(vec3(-1000.0, 0.0, 0.0));
			gl::vertex(vec3(1000.0, 0.0, 0.0));
			gl::end();

			gl::color(ColorA(0, 1, 0, 1.0f));
			gl::begin(GL_LINE_STRIP);
			gl::vertex(vec3(0.0, -1000.0, 0.0));
			gl::vertex(vec3(0.0, 1000.0, 0.0));
			gl::end();

			gl::color(ColorA(0, 0, 1, 1.0f));
			gl::begin(GL_LINE_STRIP);
			gl::vertex(vec3(0.0, 0.0, -1000.0));
			gl::vertex(vec3(0.0, 0.0, 1000.0));
			gl::end();
		}

        void drawLine(vec3 mPointA, vec3 mPointB, vec3 mColor, float mOpacity, float mLineWidth)
        {
            gl::ScopedColor scp;

            gl::lineWidth(mLineWidth);
            gl::color(ColorA(mColor.x, mColor.y, mColor.z, mOpacity));
            gl::begin(GL_LINE_STRIP);
            gl::vertex(mPointA);
            gl::vertex(mPointB);
            gl::end();
        }

        void saveImage()
        {
            using namespace ci::app;
            using namespace std;

            time_t now = time(0);
            tm* ltm = localtime(&now);
            string fileName = toString(1900 + ltm->tm_year) + '.' + toString(1 + ltm->tm_mon) + '.' + toString(ltm->tm_mday) + '-' + toString(1 + ltm->tm_hour) + '.' + toString(1 + ltm->tm_min) + '.' + toString(1 + ltm->tm_sec) + ".png";
            writeImage(getHomeDirectory() / "Downloads" / fileName, copyWindowSurface());
            CI_LOG_I("Save screenshot : " << fileName);
        }

        void drawBall(vec3 mPos, vec3 mSize, vec3 mColor, float mOpacity)
        {
            if (!bBall) {
                auto sphere = gl::VboMesh::create(geom::Sphere());
                mShaderBall = gl::GlslProg::create(gl::GlslProg::Format()
                    .vertex(CI_GLSL(100, precision highp float;

                uniform mat4    ciModelViewProjection;
                uniform mat3    ciNormalMatrix;

                attribute vec4        ciPosition;
                attribute vec2        ciTexCoord0;
                attribute vec3        ciNormal;

                uniform vec3 uPosition;
                uniform vec3 uScale;

                varying highp vec3    Normal;
                varying highp vec2    TexCoord0;

                void main(void)
                {
                    vec4 pos = ciPosition;
                    pos.xyz *= uScale;
                    pos.xyz += uPosition;

                    gl_Position = ciModelViewProjection * pos;
                    TexCoord0 = ciTexCoord0;
                    Normal = ciNormalMatrix * ciNormal;
                }))
                    .fragment(CI_GLSL(100, precision mediump float;

                precision highp float;

                varying vec3 Normal;


                uniform vec3 uColor;
                uniform float uOpacity;

                const vec3 LIGHT = vec3(0.2, 1.0, 0.6);

                void main(void)
                {
                    float d = max(dot(normalize(Normal), normalize(LIGHT)), 0.0);
                    d = mix(d, 1.0, .25);
                    gl_FragColor = vec4(uColor * d, uOpacity);
                })));
                bBall = gl::Batch::create(sphere, mShaderBall);
            }

            gl::ScopedGlslProg progColor(mShaderBall);
            mShaderBall->uniform("uPosition", mPos);
            mShaderBall->uniform("uScale", mSize);
            mShaderBall->uniform("uColor", mColor);
            mShaderBall->uniform("uOpacity", mOpacity);

            bBall->draw();
        }

        void drawDotPlanes(float mGap)
        {
            using namespace std;
            if (!bDotsPlane) {
                vector<vec3> points;

                for (int i = 0; i < NUM_DOTS; i++) {
                    for (int j = 0; j < NUM_DOTS; j++) {
                        points.push_back(vec3(i - NUM_DOTS / 2, j - NUM_DOTS / 2, 0.0f));
                        points.push_back(vec3(i - NUM_DOTS / 2, 0.0f, j - NUM_DOTS / 2));
                    }
                }


                gl::VboRef mParticleVbo = gl::Vbo::create(GL_ARRAY_BUFFER, points, GL_STATIC_DRAW);

                // Describe particle semantics for GPU.
                geom::BufferLayout particleLayout;
                particleLayout.append(geom::Attrib::POSITION, 3, sizeof(vec3), 0);

                auto mesh = gl::VboMesh::create(int(points.size()), GL_POINTS, { { particleLayout, mParticleVbo } });

                mShaderDots = gl::GlslProg::create(gl::GlslProg::Format()
                    .vertex(CI_GLSL(100, precision highp float;

                uniform mat4        ciModelViewProjection;
                uniform float       uGap;
                attribute vec4      ciPosition;


                void main(void)
                {
                    vec4 pos = ciPosition;
                    pos.xyz *= uGap;
                    gl_Position = ciModelViewProjection * pos;
                    gl_PointSize = 2.0;
                }))
                    .fragment(CI_GLSL(100, precision mediump float;

                void main(void)
                {
                    gl_FragColor = vec4(1.0, 1.0, 1.0, 0.5);
                })));
                bDotsPlane = gl::Batch::create(mesh, mShaderDots);
            }

            gl::ScopedGlslProg progColor(mShaderDots);
            mShaderDots->uniform("uGap", mGap);
            bDotsPlane->draw();
        }

	}

}


