#ifndef EaseNumber_hpp
#define EaseNumber_hpp

#include <stdio.h>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace alfrid {
  typedef std::shared_ptr<class EaseNumber> EaseNumberRef;

  class EaseNumber {
    private:
        float _value;
        float _target;
        float _min;
        float _max;
        bool _hasSetLimit = false;

        
    public:
        float easing = 0.1;
        
        EaseNumber(float mValue, float mEasing=0.1) {
            easing = mEasing;
            setTo(mValue);

            App::get()->getSignalUpdate().connect(std::bind(&EaseNumber::update, this));
        }
        
        
        void update() {
            checkLimit();
            _value += (_target - _value) * easing;
        }

        
        void setTo(float mValue) {
            _target = _value = mValue;
        }


        void add(float mValue) {
            _target += mValue;
        }


        void limit(float min, float max) {
            if(min > max) {
                limit(max, min);
                return;
            }

            _min = min;
            _max = max;
            _hasSetLimit = true;
        }


        void checkLimit() {
            if(!_hasSetLimit) return;

            _target = max(_min, _target);
            _target = min(_max, _target);
        }
        
        
        float getValue() {
            return _value;
        }


        float getTargetValue() {
            return _target;
        }

        
        void setValue(float mValue) {
            _target = mValue;
        }

        void destroy() { 
        }
        
        
        static EaseNumberRef create(float mValue) { return std::make_shared<EaseNumber>(mValue); }

    };

}

#endif /* EaseNumber_h */