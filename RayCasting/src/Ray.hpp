//
//  Ray.hpp
//  RayCasting
//
//  Created by Yi-Wen LIN on 21/04/2020.
//

#ifndef Ray_hpp
#define Ray_hpp

#include <stdio.h>

using namespace ci;


typedef std::shared_ptr<class Ray> RayRef;


class Ray {
    
public:
    vec3 origin;
    vec3 direction;
    
    Ray() {
        
    }
//    static RayRef create() { return std::make_shared<Ray>(); }
    
    
};


#endif /* Ray_hpp */
