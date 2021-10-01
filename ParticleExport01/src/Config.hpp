//
//  Config.hpp
//  ParticleExport01
//
//  Created by Yi-Wen LIN on 30/09/2021.
//

#ifndef Config_hpp
#define Config_hpp

#include <stdio.h>

class Config {
public:
static Config& getInstance()
{
    static Config    instance;
                        
    return instance;
}
    int NUM_PARTICLES = 128;
    vec3 center = vec3(5.0);
private:
    Config() {
        
    }
    
    Config(Config const&);              // Don't Implement
    void operator=(Config const&); // Don't implement
};


#endif /* Config_hpp */
