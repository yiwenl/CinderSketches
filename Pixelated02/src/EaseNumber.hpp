//
//  EaseNumber.hpp
//  Pixelated02
//
//  Created by Yi-Wen LIN on 20/04/2020.
//

#ifndef EaseNumber_hpp
#define EaseNumber_hpp

#include <stdio.h>

typedef std::shared_ptr<class EaseNumber> EaseNumberRef;

class EaseNumber {
private:
    float _value;
    float _target;
    
public:
    float easing = 0.1;
    
    EaseNumber(float mValue) {
        setTo(mValue);
    }
    
    
    void update() {
        if(_value == _target) {
            return;
        }
        _value += (_target - _value) * easing;
        
        if(abs(_value - _target) < 0.0001) {
            setTo(_target);
        }
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
    
    
    static EaseNumberRef create(float mValue) { return std::make_shared<EaseNumber>(mValue); }

};

#endif /* EaseNumber_hpp */
