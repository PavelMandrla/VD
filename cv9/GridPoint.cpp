//
// Created by pavel on 13.11.21.
//

#include "GridPoint.h"
#include "defines.h"

float GridPoint::W_ij(float r, float h) {      // CUBIC SPLINE KERNEL FUNCTION
    float q = r / h;
    assert(q > 0);
    if (q > 2) return 0.0f;

    auto a = float(3.0f / (2.0f * M_1_PI * pow(h, 3)));
    auto b = float(q < 1 ? 2.0f / 3.0f + pow(q, 2) + pow(q, 3) / 2 : pow(2 - q, 3) / 6);
    return a * b;
}

float GridPoint::dist(Particle &p) const {
    return float(sqrt(pow(this->x - p.position_x, 2) + pow(this->y - p.position_y, 2) + pow(this->z - p.position_z, 2)));
}

void GridPoint::addParticlesEffect(Particle &particle) {
    this->value += particle.rho * W_ij(this->dist(particle), SMOOTHING_FACTOR);
}
