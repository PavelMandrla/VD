#include <iostream>

#include "Particle.h"
#include "Grid.h"
#include <vtkXMLPolyDataWriter.h>
#include "GridPoint.h"
#include "GridCell.h"
#include "Options.h"

int main(int argc, char *argv[]) {
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
        Grid grid(particles);

        writer->SetInputData(grid.triangulate());
        writer->SetFileName((to_string(i) + ".vtp").c_str());
        writer->Write();

        cout << "frame calculation finished" << endl;
    }
    free(particles);

    return 0;
}

