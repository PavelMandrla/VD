#include <iostream>

#include "Particle.h"
#include "Grid.h"
#include <vtkXMLPolyDataWriter.h>
#include "GridPoint.h"
#include "GridCell.h"

int main() {
    /*
    vtkNew<vtkXMLPolyDataWriter> writer;

    GridCell cell(
            make_shared<GridPoint>(0, 0, 0, 10),
            make_shared<GridPoint>(0, 1, 0, 0),
            make_shared<GridPoint>(1, 1, 0, 0),
            make_shared<GridPoint>(1, 0, 0, 0),

            make_shared<GridPoint>(0, 0, 1, 0),
            make_shared<GridPoint>(0, 1, 1, 0),
            make_shared<GridPoint>(1, 1, 1, 10),
            make_shared<GridPoint>(1, 0, 1, 0)
            );
    writer->SetInputData(cell.triangulate());
    writer->SetFileName("help.vtp");
    writer->Write();
    */

    auto frames = Particle::loadParticles("../data/");

    vtkNew<vtkXMLPolyDataWriter> writer;

    //#pragma omp parallel for // NOLINT(openmp-use-default-none)
    for (int i = 0; i < frames.size(); i++) {
        Grid grid(frames[i]);

        writer->SetInputData(grid.triangulate());
        writer->SetFileName((to_string(i) + ".vtp").c_str());
        writer->Write();

        cout << "frame calculation finished" << endl;
        free(frames[i]);
    }

    return 0;
}

