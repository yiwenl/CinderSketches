//
//  Utils.hpp
//  BlackHole02
//
//  Created by Yi-Wen LIN on 11/05/2020.
//

#ifndef Utils_hpp
#define Utils_hpp


#include <stdio.h>
#include "BatchHelpers.h"
#include "CinderARKit.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Utils {
public:
    static Ray generateRay(mat4 view, mat4 proj, vec3 posCam, vec2 pos) {
        
        
        mat4 mInverseViewProj = proj * view;
        glm::inverse(mInverseViewProj);
        
        vec4 _dir = vec4(pos, 0.0, 1.0) * mInverseViewProj;
        vec3 dir = vec3(_dir);
        glm::normalize(dir);
        
        
        Ray ray = Ray(posCam, dir);
        
        
        return ray;
    }
    
    static bool hitTest(Ray ray, vec3 origin, vec3 normal, vec3 *hit) {
        float distance = 0.0f;
        bool hasHit = ray.calcPlaneIntersection(origin, normal, &distance);
        vec3 n = normalize(ray.getDirection());
        
        if(hasHit) {
            *hit = ray.getOrigin() + n * distance;
        }
        
        return hasHit;
    }
    
    
    static bool hitTest(Ray ray, ARKit::PlaneAnchor a, vec3 *hit) {
        
        // direction
        vec3 dir = AlfridUtils::getLookDir(a.mTransform, vec3(0.0, -1.0, 0.0));
        
        // origin
        vec4 pos = a.mTransform * vec4(a.mCenter, 1.0);
        vec3 origin = vec3(pos);
        
        float distance = 0.0f;
        bool hasHit = ray.calcPlaneIntersection(origin, dir, &distance);
        vec3 n = normalize(ray.getDirection());
        
        if(hasHit) {
            *hit = ray.getOrigin() + n * distance;
        }
        
        return hasHit;
    }
};

#endif /* Utils_hpp */
