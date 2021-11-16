//
// Created by pavel on 13.11.21.
//

#ifndef CV9_GRID_H
#define CV9_GRID_H

#include "Particle.h"
#include "GridPoint.h"
#include "GridCell.h"
#include <memory>

using namespace std;

class Grid {
private:
    vector<vector<vector<shared_ptr<GridPoint>>>> gridPoints;
    void calculateValues(Particle* particles);
public:

    explicit Grid(Particle* particles);

    shared_ptr<GridCell> getCell(int x, int y, int z);

    float getGridMax();

    vtkSmartPointer<vtkPolyData> triangulate();

};


#endif //CV9_GRID_H
