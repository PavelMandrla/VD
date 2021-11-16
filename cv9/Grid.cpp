//
// Created by pavel on 13.11.21.
//

#include "Grid.h"
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

Grid::Grid(Particle *particles) {
    this->gridPoints = vector<vector<vector<shared_ptr<GridPoint>>>>(81,vector<vector<shared_ptr<GridPoint>>>(81,vector<shared_ptr<GridPoint>>(81,nullptr)));

    float dp = 1.0f / 81.0f;

    #pragma omp parallel for // NOLINT(openmp-use-default-none)
    for (int x_i = 0; x_i < 81; x_i++) {
        auto pos_x = float(x_i) * dp - 0.5f;
        for (int y_i = 0; y_i < 81; y_i++) {
            auto pos_y = float(y_i) * dp - 0.5f;
            for (int z_i = 0; z_i < 81; z_i++) {
                auto pos_z = float(z_i) * dp;

                this->gridPoints[x_i][y_i][z_i] = make_shared<GridPoint>(pos_x, pos_y, pos_z);
            }
        }
    }
    calculateValues(particles);
}

void Grid::calculateValues(Particle* particles) {
    float r = 2.0f / SMOOTHING_FACTOR;

    #pragma omp parallel for // NOLINT(openmp-use-default-none)
    for (int i = 0; i < PARTICLE_COUNT; i++) {


        auto p = particles[i];

        // CALCULATE BOX AROUND PARTICLE IN WHICH IT HAS AN EFFECT
        int min_x = ceil(80 * (p.position_x - r + 1.0f) / 2.0f);
        if (min_x < 0) min_x = 0;
        int max_x = floor(80 * (p.position_x + r + 1.0f) / 2.0f);
        if (max_x > 80) max_x = 80;

        int min_y = ceil(80 * (p.position_y - r + 1.0f) / 2.0f);
        if (min_y < 0) min_y = 0;
        int max_y = floor(80 * (p.position_y + r + 1.0f) / 2.0f);
        if (max_y > 80) max_y = 80;

        int min_z = ceil(80 * (p.position_z - r));
        if (min_z < 0) min_z = 0;
        int max_z = floor(80 * (p.position_z + r));
        if (max_z > 80) max_z = 80;

        // APPLY PARTICLES EFFECT TO GRID POINTS
        for (int x = min_x; x <= max_x; x++) {
            for (int y = min_y; y <= max_y; y++) {
                for (int z = min_z; z <= max_z; z++) {
                    this->gridPoints[x][y][z]->addParticlesEffect(p);
                }
            }
        }
    }
}

shared_ptr<GridCell> Grid::getCell(int x, int y, int z) {
    assert(x >= 0 && x < 80);
    assert(y >= 0 && y < 80);
    assert(z >= 0 && z < 80);

    return make_shared<GridCell>(
            this->gridPoints[ x ][ y ][ z ],
            this->gridPoints[x+1][ y ][ z ],
            this->gridPoints[x+1][y+1][ z ],
            this->gridPoints[ x ][y+1][ z ],
            this->gridPoints[ x ][ y ][z+1],
            this->gridPoints[x+1][ y ][z+1],
            this->gridPoints[x+1][y+1][z+1],
            this->gridPoints[ x ][y+1][z+1]
            /*
            this->gridPoints[ x ][y+1][ z ],
            this->gridPoints[x+1][y+1][ z ],
            this->gridPoints[x+1][ y ][ z ],
            this->gridPoints[ x ][ y ][ z ],
            this->gridPoints[ x ][y+1][z+1],
            this->gridPoints[x+1][y+1][z+1],
            this->gridPoints[x+1][ y ][z+1],
            this->gridPoints[ x ][ y ][z+1]
             */
            // TODO -> tahle je to v prezentaci, ale nemám ponětí, jestli to je dobře
            // TODO -> ASI TO DĚLEJ PODLE http://paulbourke.net/geometry/polygonise/
            /*
            this->gridPoints[ x ][ y ][z+1],
            this->gridPoints[x+1][ y ][z+1],
            this->gridPoints[x+1][y+1][z+1],
            this->gridPoints[ x ][y+1][z+1],
            this->gridPoints[ x ][ y ][ z ],
            this->gridPoints[x+1][ y ][ z ],
            this->gridPoints[x+1][y+1][ z ],
            this->gridPoints[ x ][y+1][ z ]
             */
            );
}

float Grid::getGridMax() {
    float max = 0.0f;
    for (int x = 0; x < 81; x++) {
        for (int y = 0; y < 81; y++) {
            for (int z = 0; z < 81; z++) {
                if (gridPoints[x][y][z]->value > max)
                    max = gridPoints[x][y][z]->value;
            }
        }
    }
    return max;
}

vtkSmartPointer<vtkPolyData> Grid::triangulate() {
    vtkNew<vtkAppendPolyData> appendFilter;
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 80; y++) {
            for (int z = 0; z < 80; z++) {
                //auto polydata = this->getCell(x, y, z)->triangulate();
                appendFilter->AddInputData(this->getCell(x, y, z)->triangulate());
            }
        }
    }
    appendFilter->Update();

    vtkNew<vtkCleanPolyData> cleanFilter;
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    cleanFilter->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(cleanFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkPolyData> part1 = vtkPolyData::SafeDownCast(actor->GetMapper()->GetInputAsDataSet());

    return part1;
}

