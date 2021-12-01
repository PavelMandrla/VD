//
// Created by pavel on 13.11.21.
//

#ifndef CV9_PARTICLE_H
#define CV9_PARTICLE_H


#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include "half.hpp"


using namespace std;
using namespace half_float;

struct Particle {
    half position_x; // particle position (m)
    half position_y;
    half position_z;

    half velocity_x; // particle velocity (m/s)
    half velocity_y;
    half velocity_z;

    half rho; // density (kg/m3)
    half pressure;
    half radius; // particle radius (m)
};

#endif //CV9_PARTICLE_H
