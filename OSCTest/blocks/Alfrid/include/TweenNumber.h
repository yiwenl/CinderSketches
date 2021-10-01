//
//  TweenNumber.h
//  AlfridTool
//
//  Created by Yi-Wen LIN on 03/06/2020.
//

#ifndef TweenNumber_h
#define TweenNumber_h

#include <stdio.h>
#include "cinder/Easing.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace alfrid {

std::function<float (float)> getFunc(string mName) {
    std::function<float (float)> _func = easeNone;
    
    // Cubic
    if(mName == "easeInCubic") {
        _func = easeInCubic;
    } else if(mName == "easeOutCubic") {
        _func = easeOutCubic;
    } else if(mName == "easeInOutCubic") {
        _func = easeInOutCubic;
    } else if(mName == "easeOutInCubic") {
        _func = easeOutInCubic;
    }
    // Sine
    else if(mName == "easeInSine") {
        _func = easeInSine;
    } else if(mName == "easeOutSine") {
        _func = easeOutSine;
    } else if(mName == "easeInOutSine") {
        _func = easeInOutSine;
    } else if(mName == "easeOutInSine") {
        _func = easeOutInSine;
    }
    // Circ
    else if(mName == "easeInCirc") {
        _func = easeInCirc;
    } else if(mName == "easeOutCirc") {
        _func = easeOutCirc;
    } else if(mName == "easeInOutCirc") {
        _func = easeInOutCirc;
    } else if(mName == "easeOutInCirc") {
        _func = easeOutInCirc;
    }
    // Expo
    else if(mName == "easeInExpo") {
        _func = easeInExpo;
    } else if(mName == "easeOutExpo") {
        _func = easeOutExpo;
    } else if(mName == "easeInOutExpo") {
        _func = easeInOutExpo;
    } else if(mName == "easeOutInExpo") {
        _func = easeOutInExpo;
    }
    // Quad
    else if(mName == "easeInQuad") {
        _func = easeInQuad;
    } else if(mName == "easeOutQuad") {
        _func = easeOutQuad;
    } else if(mName == "easeInOutQuad") {
        _func = easeInOutQuad;
    } else if(mName == "easeOutInQuad") {
        _func = easeOutInQuad;
    }
    // Quart
    else if(mName == "easeInQuint") {
        _func = easeInQuint;
    } else if(mName == "easeOutQuint") {
        _func = easeOutQuint;
    } else if(mName == "easeInOutQuint") {
        _func = easeInOutQuint;
    } else if(mName == "easeOutInQuint") {
        _func = easeOutInQuint;
    }
    // Quart
    else if(mName == "easeInQuart") {
        _func = easeInQuart;
    } else if(mName == "easeOutQuart") {
        _func = easeOutQuart;
    } else if(mName == "easeInOutQuart") {
        _func = easeInOutQuart;
    } else if(mName == "easeOutInQuart") {
       _func = easeOutInQuart;
   }
    // Bounce
    else if(mName == "easeInBounce") {
        _func = EaseInBounce();
    } else if(mName == "easeOutBounce") {
        _func = EaseOutBounce();
    } else if(mName == "easeInOutBounce") {
        _func = EaseInOutBounce();
    } else if(mName == "easeOutInBounce") {
       _func = EaseOutInBounce();
   }
    // Back
    else if(mName == "easeInBack") {
        _func = EaseInBack();
    } else if(mName == "easeOutBack") {
        _func = EaseOutBack();
    } else if(mName == "easeInOutBack") {
        _func = EaseInOutBack();
    } else if(mName == "easeOutInBack") {
        _func = EaseOutInBack();
    }
    // Elastic
    else if(mName == "easeInElastic") {
        _func = EaseInElastic(3, 1);
    } else if(mName == "easeOutElastic") {
        _func = EaseOutElastic(1, 3);
    } else if(mName == "easeInOutElastic") {
        _func = EaseInOutElastic(3, 1);
    } else if(mName == "easeOutInElastic") {
        _func = EaseOutInElastic(1, 3);
    }
    else {
        CI_LOG_D("No easing found :" << mName);
    }
    
    return _func;
}

    
class TweenNumber {
public:
    TweenNumber(float mValue, string mEasing="linear", float mSpeed = 0.01) {
        easing = mEasing;
        speed = mSpeed;
        
        _start = mValue;
        _value = mValue;
        _target = mValue;
        
        App::get()->getSignalUpdate().connect(std::bind(&TweenNumber::_update, this));
    }
    
    void setTo(float mValue) {
        _p = 1.0f;
        _value = _target = mValue;
    }
    
    float getValue() {  return _value;  }
    void setValue(float mValue) {
        _p = 0.0f;
        _start = _value;
        _target = mValue;
    }
    
    float getTarget() {
        return _target;
    }
    
    string easing;
    float speed;

private:
    float _value;
    float _start;
    float _target;
    float _p = 1.0f;
    
    void _update() {
        _p += speed;
        _p = min(_p, 1.0f);
        
        std::function<float (float)> func = getFunc(easing);
        float t = func(_p);
        
        _value = mix(_start, _target, t);
        
//        CI_LOG_D("Update : " << _p << ", " << t << ", " << _start << " / " << _target);
    }
};

}

#endif /* TweenNumber_h */
