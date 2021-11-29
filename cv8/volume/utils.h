//
// Created by pavel on 10.11.21.
//

#ifndef VOLUMETRICDATA_UTILS_H
#define VOLUMETRICDATA_UTILS_H

#include "vector3.h"

float sigmoid(float x, const float c1 = 0.05f, const float c2 = 127.5f, const float max = 255.0f) {
    return max / (1 + exp(-c1 * (x - c2)));
}

float transfer_function(float x, float px[4], float py[2]) {
    if (x <= px[0] || x >= px[3]) return py[0];
    if (x >= px[1] && x <= px[2]) return py[1];
    if (x < px[1])
        return py[0] + (x-px[0])*(px[1]-px[0]) * (py[1]-py[0]);
    return py[1] - (x-px[2])*(px[3]-px[2]) * (py[1]-py[0]);
}

Vector3 transfer_function_color(float x) {
    float px_r[4] = {0.0f, 0.2f, 0.3f, 0.5f};
    float px_g[4] = {0.2f, 0.5f, 0.7f, 0.9f};
    float px_b[4] = {0.7f, 0.9f, 1.0f, 1.5f};
    float py[2] = {0.0f, 1.0f};

    return Vector3 {
            transfer_function(x, px_r, py), // TODO - maybe change to BGR
            transfer_function(x, px_g, py),
            transfer_function(x, px_b, py)
    };
}


#endif //VOLUMETRICDATA_UTILS_H
