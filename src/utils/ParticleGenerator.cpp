#include "ParticleGenerator.h"
#include "utils/ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

void ParticleGenerator::generateParticles(ParticleContainer &particles, const std::string &file) {
    std::vector<ShapeInfo> infovec = readJSON(file);
    for(auto &info : infovec){
        switch (info.type){
            case cuboid:
                generateCuboid(particles, info);
                break;
            case sphere:
                generateSphere(particles, info);
                break;
        }
    }
}

std::vector<ParticleGenerator::ShapeInfo> ParticleGenerator::readJSON(const std::string &file){
    std::ifstream input_file(file);
    nlohmann::json j;
    std::vector<ParticleGenerator::ShapeInfo> vec{};
    // load data from file to json object
    input_file >> j;

    for (auto &shape : j["shapes"]){
        std::string type = shape["type"];
        ShapeInfo i{};

        i.pos = shape["pos"];
        i.vel = shape["vel"];
        i.distance = shape["distance"];
        i.mass = shape["mass"];
        i.brownianFactor = shape["brownianFactor"];
        i.brownianDIM = shape["brownianDIM"];

        if(type == "cuboid"){
            i.type = ParticleGenerator::cuboid;
            i.N = shape["N"];
        }else if(type == "sphere"){
            i.type = ParticleGenerator::cuboid;
            i.radius = shape["radius"];
        }else{
            std::cerr << "Invalid Shape type" << std::endl;
            continue;
        }
        // add parsed shape info to vector
        vec.emplace_back(i);
    }
    return vec;
}

void ParticleGenerator::generateCuboid(ParticleContainer &particles, const ShapeInfo &info) {
    // reserve amount of particles we are going to create
    int total_count = info.N[0] * info.N[1] * info.N[2];
    particles.reserve(total_count);

    std::array<double, 3> currentPos{info.pos};

    for(int z = 0; z < info.N[2]; z++){
        for(int y = 0; y < info.N[1]; y++){
            for(int x = 0; x < info.N[0]; x++){
                Particle part;
                // TODO check if this is correct
                // add browian motion
                auto tempVel = info.vel + maxwellBoltzmannDistributedVelocity(info.brownianFactor, info.brownianDIM);
                part = Particle{currentPos, tempVel, info.mass};

                particles.emplace_back(part);
                currentPos[0] += info.distance;
            }
            currentPos[0] = info.pos[0];
            currentPos[1] += info.distance;
        }
        currentPos[0] = info.pos[0];
        currentPos[1] = info.pos[1];
        currentPos[2] += info.distance;
    }
}

// TODO
void ParticleGenerator::generateSphere(ParticleContainer &particles, const ShapeInfo &info) {
    // dummy particle so anything shows up
    particles.emplace_back(Particle{info.pos, info.vel, info.mass});
}