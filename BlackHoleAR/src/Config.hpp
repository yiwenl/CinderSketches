//
//  Config.hpp
//  BlackHoleAR
//
//  Created by Yi-Wen LIN on 31/01/2020.
//

#ifndef Config_hpp
#define Config_hpp

#include <stdio.h>

#endif /* Config_hpp */


class Config {
public:
static Config& getInstance()
{
    static Config    instance; // Guaranteed to be destroyed.
                          // Instantiated on first use.
    return instance;
}
    int NUM_PARTICLES = 120e3;
private:
    Config() {
        
    }
    
    Config(Config const&);              // Don't Implement
    void operator=(Config const&); // Don't implement
};
