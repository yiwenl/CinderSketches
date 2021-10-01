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
        
    public:
        float easing = 0.1;
        
        EaseNumber(float mValue, float mEasing=0.1) {
            easing = mEasing;
            setTo(mValue);

            App::get()->getSignalUpdate().connect(std::bind(&EaseNumber::update, this));
        }
        
        
        void update() {
            _value += (_target - _value) * easing;
        }
        
        void setTo(float mValue) {
            _target = _value = mValue;
        }
        
        
        float getValue() {
            return _value;
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