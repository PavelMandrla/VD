//
// Created by pavel on 17.11.21.
//

#ifndef CV9_OPTIONS_H
#define CV9_OPTIONS_H

#include <cxxopts.hpp>
#include <string>

using namespace std;

class Options : cxxopts::Options {
private:
    static Options* instance;

    int from = 0;
    int to = 499;
    string srcDir = "../data/";
    int particleCount = 40*40*40;
    float smoothingFactor = 25.0f;
    float threshold = 2.0f;

    void registerOptions();
    Options(int argc, char *argv[]);
public:
    static void init(int argc, char *argv[]);

    static Options* getInstance();

    int getFrom() const;

    int getTo() const;

    const string &getSrcDir() const;

    int getParticleCount() const;

    float getSmoothingFactor() const;

    float getThreshold() const;

};


#endif //CV9_OPTIONS_H
