//
// Created by pavel on 14.11.21.
//

#ifndef CV9_GRIDCELL_H
#define CV9_GRIDCELL_H

#include <array>
#include <list>
#include <memory>
#include <utility>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include "GridPoint.h"

using namespace std;

typedef shared_ptr<GridPoint> GPP;



class GridCell {
private:
    array<shared_ptr<GridPoint>, 8> vertices;

    int getCubeIndex();

    //vtkSmartPointer<vtkPoints> getPoints(int cubeIndex);

    //vtkSmartPointer<vtkCellArray> getTriangles(int cubeIndex);

public:
    GridCell(GPP p1, GPP p2, GPP p3, GPP p4, GPP p5, GPP p6, GPP p7, GPP p8) : vertices({p1, p2, p3, p4, p5, p6, p7, p8}) {}

    vtkSmartPointer<vtkPolyData> triangulate();

};


#endif //CV9_GRIDCELL_H
