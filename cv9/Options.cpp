//
// Created by pavel on 17.11.21.
//

#include "Options.h"
#include <exception>

Options::Options(int argc, char **argv) : cxxopts::Options("marching cubes") {
    this->registerOptions();

    auto result = this->parse(argc, argv);
    if (result.count("help")) {
        std::cout << this->help({""}) << std::endl;
        exit(0);
    }

    if (result.count("path"))
        this->srcDir = result["path"].as<string>();
}

void Options::registerOptions() {
    this->add_options()
            ("help", "Print help")
            ("from", "number of starting file", cxxopts::value<int>(this->from))
            ("to", "number of ending file", cxxopts::value<int>(this->to))
            ("path", "path to directory with input files", cxxopts::value<std::string>())
            ("particle_count", "number of particles in the input files", cxxopts::value<int>(this->particleCount))
            ("smoothing_factor", "ammount of smoothing", cxxopts::value<float>(this->smoothingFactor))
            ("threshold", "threshold of the isosurface", cxxopts::value<float>(this->threshold))
            ;
}

int Options::getFrom() const {
    return from;
}

int Options::getTo() const {
    return to;
}

const string &Options::getSrcDir() const {
    return srcDir;
}

int Options::getParticleCount() const {
    return particleCount;
}

float Options::getSmoothingFactor() const {
    return smoothingFactor;
}

float Options::getThreshold() const {
    return threshold;
}

void Options::init(int argc, char *argv[]) {
    Options::instance = new Options(argc, argv);
}

Options *Options::getInstance() {
    if (Options::instance == nullptr)
        throw std::exception();
    return Options::instance;
}

Options* Options::instance = nullptr;