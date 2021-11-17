#include <iostream>

#include "Particle.h"
#include "Grid.h"
#include <vtkXMLPolyDataWriter.h>
#include "GridPoint.h"
#include "GridCell.h"
#include "Options.h"

int main(int argc, char *argv[]) {

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

    Options::init(argc, argv);
    auto opts = Options::getInstance();
    vtkNew<vtkXMLPolyDataWriter> writer;
    auto particles = (Particle*) malloc(opts->getParticleCount() * sizeof(Particle));

    //#pragma omp parallel for // NOLINT(openmp-use-default-none)
    for (int i = opts->getFrom(); i <= opts->getTo(); i++) {
        // LOAD PARTICLES FROM BINARY FILE


        stringstream ss;
        ss << opts->getSrcDir();
        ss << setw(3) << setfill('0') << i << "0.bin";
        string path = ss.str();

        ifstream inFile(path);
        if (!inFile.is_open()) {
            cout << "failed to open " << path << endl;
        }
        inFile.read((char*) particles, opts->getParticleCount() * sizeof(Particle));


        // MARCHING CUBES
        //auto grid = make_shared<Grid>(particles);
        Grid grid(particles);


        writer->SetInputData(grid.triangulate());
        writer->SetFileName((to_string(i) + ".vtp").c_str());
        writer->Write();

        cout << "frame calculation finished" << endl;
    }
    free(particles);

    return 0;
}

