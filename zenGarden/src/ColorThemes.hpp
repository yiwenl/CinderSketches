//
//  ColorThemes.hpp
//  zenGarden
//
//  Created by Yi-Wen LIN on 02/05/2020.
//

#ifndef ColorThemes_hpp
#define ColorThemes_hpp

#include <stdio.h>
#include "cinder/Rand.h"
#include <algorithm>
#include <random>

using namespace ci;
using namespace ci::app;
using namespace std;


namespace ColorThemes {

vector<vector<int>> _colorThemes {
    {0x112F41, 0x068587, 0x4FB99F, 0xF2B134, 0xED553B},
    {0xF23C50, 0xFFCB05, 0xE9F1DF, 0x4AD9D9, 0x36B1BF},
    {0x120D1E, 0x721825, 0x7E9DA4, 0xD8D0C5, 0x1B1414},
    {0x233656, 0x415B76, 0x7B9BA6, 0xCDD6D5, 0xEEF4F2},
    {0xF2BE54, 0xCDD4CA, 0xCDD4CA, 0x153E5C, 0x191A1D},
    {0xF5F4EC, 0x56638A, 0x42BA78, 0xFAC8CD, 0xF3B918},
    {0xCEF19E, 0xA7DDA7, 0x78BE97, 0x398689, 0x0B476D},
    {0x7B8D6D, 0xFEAE94, 0xFF8F7A, 0x7B403D, 0xCD5B56},
    {0xF8B195, 0xF67280, 0xC06C84, 0x6C5B7B, 0x355C7D},
    {0xCBD9D6, 0x8DA593, 0xD9CBA3, 0xF2A88C, 0xD98282},
    {0xE6E2AF, 0xA7A37E, 0xEFECCA, 0x046380, 0x002F2F},
    {0x2C3E50, 0xE74C3C, 0xECF0F1, 0x3498DB, 0x2980B9},
    {0x225378, 0x1695A3, 0xACF0F2, 0xF3FFE2, 0xEB7F00},
    {0x468966, 0xFFF0A5, 0xFFB03B, 0xB64926, 0x8E2800},
    {0x004358, 0x1F8A70, 0xBEDB39, 0xFFE11A, 0xFD7400},
    {0xDC3522, 0xD9CB9E, 0x374140, 0x2A2C2B, 0x1E1E20},
    {0xB9121B, 0x4C1B1B, 0xF6E497, 0xFCFAE1, 0xBD8D46},
    {0x2E0927, 0xD90000, 0xFF2D00, 0xFF8C00, 0x04756F},
};

vector<vec3> getColor(void);

vector<vec3> getColor() {
    srand(unsigned(time(NULL)));
    int i = floor(randFloat(_colorThemes.size()));
    
    console() << "Color Index : " << i << endl;
    
    auto hexValues = _colorThemes.at(i);
    
    srand(unsigned(time(NULL)));
    std::random_shuffle(hexValues.begin(), hexValues.end());
    
    float exposure = 2.2;

    vector<vec3> colors;
    for( auto hexValue : hexValues) {
        float r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
        float g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
        float b = ((hexValue) & 0xFF) / 255.0;
        
        r = pow(r, 1.0/exposure);
        g = pow(g, 1.0/exposure);
        b = pow(b, 1.0/exposure);
        
        colors.push_back(vec3(r, g, b));
    }
    
    return colors;
}


}


#endif /* ColorThemes_hpp */
