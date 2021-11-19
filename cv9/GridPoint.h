//
// Created by pavel on 13.11.21.
//

#ifndef CV9_GRIDPOINT_H
#define CV9_GRIDPOINT_H

#include "Particle.h"


class GridPoint {
private:
    static float W_ij(float r, float h);
public:
    float value;
    float x, y, z;

    float dist(Particle &p) const;

    //GridPoint(Particle* particles, float x, float y, float z) : value(0.0f), x(x), y(y), z(z) {}
    GridPoint() : value(0.0f), x(0), y(0), z(0) {};
    GridPoint(float x, float y, float z) : value(0.0f), x(x), y(y), z(z) {}
    GridPoint(float x, float y, float z, float value) : value(value), x(x), y(y), z(z) {}

    void addParticlesEffect(Particle &particle);
};

#endif //CV9_GRIDPOINT_H
