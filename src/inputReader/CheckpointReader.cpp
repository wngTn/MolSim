#include <fstream>
#include <iostream>
#include "CheckpointReader.h"

#include "nlohmann/json.hpp"

void CheckpointReader::readCheckpoint(std::string& inputFile, ParticleContainer& particles, std::vector<std::pair<int, std::pair<double, double>>>& mapping, Config& config) {
    std::ifstream input{inputFile};
    nlohmann::json j;
    input >> j;

    config.start_iteration = j["iterations"];
    for(auto& m : j["calc"]["mapping"]){
        auto pair = std::make_pair(m[0], std::make_pair(m[1][0], m[1][1]));
        mapping.emplace_back(pair);
    }

    for (auto &p : j["particles"]){
        particles.push_back(p.get<Particle>());
    }


}
