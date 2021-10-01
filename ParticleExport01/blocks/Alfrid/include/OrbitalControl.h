//
//  OrbitalControl.h
//  OrbitalControl
//
//  Created by Yi-Wen LIN on 17/06/2020.
//

#ifndef OrbitalControl_h
#define OrbitalControl_h

#include <stdio.h>
#include "cinder/app/App.h"
#include "cinder/Camera.h"
#include "cinder/Log.h"
#include "EaseNumber.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace alfrid {

    class OrbitalControl {
    public:
        alfrid::EaseNumberRef rx;
        alfrid::EaseNumberRef ry;
        alfrid::EaseNumberRef radius;
        
        OrbitalControl(Camera *camera, const app::WindowRef &window) : mCamera(camera), mWindow(window) {
            App::get()->getSignalUpdate().connect(std::bind(&OrbitalControl::update, this));
            
            const int signalPriority = -1;
            mWindow->getSignalMouseDown().connect( signalPriority, [this]( app::MouseEvent &event ) { mouseDown( event ); } );
            mWindow->getSignalMouseDrag().connect( signalPriority, [this]( app::MouseEvent &event ) { mouseDrag( event ); } );
            mWindow->getSignalMouseWheel().connect( signalPriority, [this]( app::MouseEvent &event ) { mouseWheel( event ); } );
            
            // for touch
            mWindow->getSignalTouchesBegan().connect( signalPriority, [this]( app::TouchEvent &event ) { touchesBegan( event ); } );
            mWindow->getSignalTouchesMoved().connect( signalPriority, [this]( app::TouchEvent &event ) { touchesMoved( event ); } );
            
            rx = alfrid::EaseNumber::create(0);
            ry = alfrid::EaseNumber::create(0);
            
            const float a = M_PI * 0.5 - 0.0001;
            rx->limit(-a, a);
            
            vec3 camPos = mCamera->getEyePoint();
            radius = alfrid::EaseNumber::create(length(camPos));
            
        }
        
        void lock(bool mValue) {
            _isLocked = mValue;
        }
        
    private:
        Camera* mCamera;
        WindowRef mWindow;
        
        float _preRX = 0;
        float _preRY = 0;
        bool _isLocked = false;
        
        vec2 _preMouse;
        
        void mouseDown(MouseEvent event) {
            onDown(event.getPos());
            event.setHandled();
        }
        
        void touchesBegan(TouchEvent event) {
            auto touch = event.getTouches().at(0);
            onDown(touch.getPos());
            event.setHandled();
        }
        
        void touchesMoved(TouchEvent event) {
            auto touch = event.getTouches().at(0);
            onDrag(touch.getPos());
            event.setHandled();
        }
        
        void mouseDrag(MouseEvent event) {
            onDrag(event.getPos());
            event.setHandled();
        }
        
        void mouseWheel(MouseEvent event) {
            float target = radius->getTargetValue();
            target += event.getWheelIncrement() * 0.3;
            radius->setValue(target);
            event.setHandled();
        }
        
        void onDown(vec2 pos) {
            _preMouse = pos;
            _preRX = rx->getTargetValue();
            _preRY = ry->getTargetValue();
        }
        
        void onDrag(vec2 mouse) {
            if(_isLocked) { return; }
            float diff = mouse.x - _preMouse.x;
            ry->setValue(_preRY + diff * 0.01);
            
            diff = mouse.y - _preMouse.y;
            rx->setValue(_preRX + diff * 0.01);
        }
        
         
        void update() {
            float r = radius->getValue();
            float a = rx->getValue();
            float y = sin(a) * r;
            float rr = cos(a) * r;
            
            float halfPI = M_PI * 0.5;
            a = ry->getValue() + halfPI;
            float x = cos(a) * rr;
            float z = sin(a) * rr;
            
            mCamera->lookAt(vec3(x, y, z), vec3(0));
        }
        
    };

}

#endif /* OrbitalControl_h */
