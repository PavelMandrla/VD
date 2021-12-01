//
// Created by pavel on 17.11.21.
//
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

int main() {
    #pragma omp parallel for num_threads(4) // NOLINT(openmp-use-default-none)
    for (int i = 0; i < 500; i += 50) {
        string command = "./cv9 --from " + to_string(i) + " --to " + to_string(i + 49);
        system(command.c_str());
    }


    return 0;
}

