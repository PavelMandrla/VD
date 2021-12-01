//
// Created by pavel on 09.11.21.
//

#ifndef VOLUMETRICDATA_CELLHIT_H
#define VOLUMETRICDATA_CELLHIT_H

struct CellIndices {
public:
    CellIndices() {
        i = -1;
        j = -1;
        k = -1;
    }

    CellIndices( const int i, const int j, const int k );

    int i;
    int j;
    int k;
};

struct CellHit {
public:
    CellHit(CellIndices & indices, const float t_0, const float t_1) {
        this->indices = indices;
        this->t_0 = t_0;
        this->t_1 = t_1;

        f = 0.0f;
    }

    CellIndices indices;

    float t_0;
    float t_1;

    float f;
};

#endif //VOLUMETRICDATA_CELLHIT_H
