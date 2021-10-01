#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;

namespace alfrid {
	namespace helpers {

		const int NUM_DOTS = 100;
		static gl::GlslProgRef  mShaderDots;
		static gl::BatchRef		bDotsPlane;

		static gl::GlslProgRef  mShaderBall;
		static gl::BatchRef		bBall;

		// axis
		void drawAxis(float mLineWidth = 1.0f);

		// grid of dots
		void drawDotPlanes(float mGap = 1.0f);

		// ball
		void drawBall(vec3 mPos, vec3 mSize = vec3(1.0), vec3 mColor = vec3(1.0), float mOpacity = 1.0f);

		// line
		void drawLine(vec3 mA, vec3 mB, vec3 mColor=vec3(1.0), float mOpacity=1.0, float mLineWidth = 1.0f);

		// save capture
		void saveImage();
	}
}