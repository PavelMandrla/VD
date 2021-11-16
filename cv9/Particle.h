//
// Created by pavel on 13.11.21.
//

#ifndef CV9_PARTICLE_H
#define CV9_PARTICLE_H


#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include "defines.h"
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

    static vector<Particle*> loadParticles(const string& dir_path) {
        vector<Particle*> result;
        for (int i = 0; i < 500; i++) {
            auto tmp = malloc(PARTICLE_COUNT * sizeof(Particle));

            stringstream ss;
            ss << dir_path;
            ss << setw(3) << setfill('0') << i << "0.bin";
            string path = ss.str();

            ifstream inFile(path);
            if (!inFile.is_open()) {
                cout << "failed to open " << path << endl;
            }
            inFile.read((char*)tmp, PARTICLE_COUNT * sizeof(Particle));

            result.push_back((Particle*) tmp);
        }
        return result;
    }
};

#endif //CV9_PARTICLE_H
