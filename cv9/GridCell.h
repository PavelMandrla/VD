//
// Created by pavel on 14.11.21.
//

#ifndef CV9_GRIDCELL_H
#define CV9_GRIDCELL_H

#include <vector>
#include <list>
#include <memory>
#include <utility>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include "GridPoint.h"

using namespace std;

class GridCell {
private:
    vector<GridPoint*> vertices;

    int getCubeIndex();

public:
    GridCell(GridPoint *p1, GridPoint *p2, GridPoint *p3, GridPoint *p4, GridPoint *p5, GridPoint *p6, GridPoint *p7, GridPoint *p8)
        : vertices({p1, p2, p3, p4, p5, p6, p7, p8}) {}

    vtkSmartPointer<vtkPolyData> triangulate();

};


#endif //CV9_GRIDCELL_H
