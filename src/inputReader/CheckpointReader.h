#pragma once
#include "utils/Config.h"

class CheckpointReader {
public:
    static void readCheckpoint(std::string& inputFile, ParticleContainer& particles, std::vector<std::pair<int, std::pair<double, double>>>& mapping, Config& config);
};
