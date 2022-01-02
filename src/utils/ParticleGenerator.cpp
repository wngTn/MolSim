#include "ParticleGenerator.h"
#include "utils/ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

std::vector<std::pair<int, std::pair<double,double>>> ParticleGenerator::generateParticles(ParticleContainer &particles, const std::vector<ShapeInfo>& infovec, int startSEIndex) {
    int index = startSEIndex;
    std::vector<std::pair<int, std::pair<double,double>>> mapping {};
    for(auto &info : infovec){
        switch (info.type){
            case cuboid:
                generateCuboid(particles, info, index);
                mapping.push_back({index, {info.epsilon, info.sigma}});
                break;
            case sphere:
                generateSphere(particles, info, index);
                mapping.push_back({index, {info.epsilon, info.sigma}});
                break;
        }
        index++;
    }
    return mapping;
}

std::vector<std::pair<int, std::pair<double,double>>> ParticleGenerator::generateParticles(ParticleContainer &particles, const std::string &file, int startSEIndex) {
    std::vector<ShapeInfo> infovec = readJSON(file);
    int index = startSEIndex;
    std::vector<std::pair<int, std::pair<double,double>>> mapping {};

    for(auto &info : infovec){
        switch (info.type){
            case cuboid:
                generateCuboid(particles, info, index);
                mapping.push_back({index, {info.epsilon, info.sigma}});
                break;
            case sphere:
                generateSphere(particles, info, index);
                mapping.push_back({index, {info.epsilon, info.sigma}});
                break;
        }
        index++;
    }
    return mapping;
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
        i.DIM = shape["DIM"];

        if(type == "cuboid"){
            i.type = ParticleGenerator::cuboid;
            i.N = shape["N"];
        }else if(type == "sphere"){
            i.type = ParticleGenerator::sphere;
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

void ParticleGenerator::generateCuboid(ParticleContainer &particles, const ShapeInfo &info, int startSEIndex) {
    // reserve amount of particles we are going to create
    int total_count = info.N[0] * info.N[1] * info.N[2];
    particles.reserve(total_count);

    std::array<double, 3> currentPos{info.pos};
    std::array<double,3> vel{};
    std::array<double,3> baseF{};
    double m;
    int current_seindex = startSEIndex;
    for(int z = 0; z < (info.DIM == 3 ? info.N[2] : 1); z++){
        for(int y = 0; y < info.N[1]; y++){
            for(int x = 0; x < info.N[0]; x++){
                Particle part{};
                // check if particle is one of the "special" particles
                auto it = std::find_if(info.specialParticles.begin(), info.specialParticles.end(),
                                       [x,y,z](auto& s){
                                           return x == std::get<0>(s)[0] && y==std::get<0>(s)[1] && z == std::get<0>(s)[2];});
                if(it != info.specialParticles.end()){
                    baseF = std::get<1>(*it);
                    vel = std::get<2>(*it);
                    m = std::get<3>(*it);
                }else{
                    vel = info.vel;
                    baseF = info.baseForce;
                    m = info.mass;
                }

                // add browian motion
                auto tempVel = vel + maxwellBoltzmannDistributedVelocity(info.brownianFactor, info.DIM);
                part = Particle{currentPos, tempVel, m, current_seindex, current_seindex};
                part.setBaseForce(baseF);

                switch(info.behaviour){
                    case membrane: { // extra scope, so index is not visible in other cases -> no initialization bypassing
                        part.setMembrane(true);
                        part.setImmovable(false);
                        auto index = std::array<int,3>{x,y,z};
                        part.setGridIndex(index);
                        break;}
                    case immovable:
                        part.setImmovable(true);
                        part.setMembrane(false);
                        break;
                    case normal:
                    default:
                        part.setMembrane(false);
                        part.setImmovable(false);
                        break;
                }

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

void ParticleGenerator::generateSphere(ParticleContainer &particles, const ShapeInfo &info, int startSEIndex) {
    // get parameters for cube generation
    std::vector<int> edges = {2*info.radius,2 * info.radius,2 * info.radius};
    std::array<double,3> cubeCorner = {info.pos[0]-(info.radius*info.distance),
                                       info.pos[1]-(info.radius*info.distance),
                                       info.pos[2]-(info.radius*info.distance)};
    auto currentPos{cubeCorner};
    if(info.DIM == 2){
        currentPos[2] = info.pos[2];
    }

    int current_seindex = startSEIndex;


    // ratio of volume of maximum inscribed sphere to cube is pi/6 ~= 0.523...
    int count = floor(pow((2*info.radius+1),3) * 0.523598775598f);
    particles.reserve(count);

    for(int z = 0; z <= (info.DIM == 3 ? edges[2] : 0); z++){
        for(int y = 0; y <= edges[1]; y++){
            for(int x = 0; x <= edges[0]; x++){
                double delta_x = currentPos[0] - info.pos[0];
                double delta_y = currentPos[1] - info.pos[1];
                double delta_z = currentPos[2] - info.pos[2];

                double distance = std::abs(sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z));
                // only add if distance to center is less or equal the radius (with rounding error tolerance)
                if(distance <= info.radius * info.distance + 0.00001){
                    Particle part;
                    auto tempVel = info.vel + maxwellBoltzmannDistributedVelocity(info.brownianFactor, info.DIM);
                    particles.emplace_back(Particle{currentPos, tempVel, info.mass, current_seindex, current_seindex});
                }
                currentPos[0] += info.distance;
            }
            currentPos[0] = cubeCorner[0];
            currentPos[1] += info.distance;
        }
        currentPos[0] = cubeCorner[0];
        currentPos[1] = cubeCorner[1];
        currentPos[2] += info.distance;
    }
}

void ParticleGenerator::generateSphere2(ParticleContainer &particles, const ShapeInfo &info) {
    // center
    // particles.emplace_back(Particle{info.pos, info.vel, info.mass});
    /* int rings = floor(info.radius / info.distance);
    float degrees = 60.0;
    for(int i = 1; i <= rings; i++){
        // generate beams from center degrees = (60/i) degree apart
    } */

    ShapeInfo i{info};
    i.type = cuboid;

    // how many particles fit on the radius between center and edge
    // int height = floor(info.radius / info.distance);
    int height = info.radius;

    i.N = {2*height + 1,2 * height + 1,2 * height + 1};
    i.distance = info.distance;
    i.pos = {-(height*info.distance),
             -(height*info.distance),
             -(height*info.distance)};
    generateCuboid(particles, i, 0);

    for(Particle &p : particles){
        // std::cout << "X before scaling: " << p.getX();
        p.setX((1/(height * info.distance)) * p.getX());
        // std::cout << "X after scaling: " << p.getX() << std::endl;
        double x2 = p.getX()[0] * p.getX()[0];
        double y2 = p.getX()[1] * p.getX()[1];
        double z2 = p.getX()[2] * p.getX()[2];

        double x = p.getX()[0] * sqrt(1 - (y2 + z2) / 2 + (y2 * z2) / 3);
        double y = p.getX()[1] * sqrt(1 - (z2 + x2) / 2 + (z2 * x2) / 3);
        double z = p.getX()[2] * sqrt(1 - (x2 + y2) / 2 + (x2 * y2) / 3);
        p.setX({x*info.radius,y*info.radius,z*info.radius});
    }
}

std::vector<std::pair<int, std::pair<double,double>>> ParticleGenerator::generateParticles(ParticleContainer &particles, const std::string &file) {
    return generateParticles(particles, file, 0);
}

std::vector<std::pair<int, std::pair<double,double>>> ParticleGenerator::generateParticles(ParticleContainer &particles, const std::vector<ShapeInfo> &infovec) {
    return generateParticles(particles, infovec, 0);
}

