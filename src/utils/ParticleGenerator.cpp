#include "ParticleGenerator.h"
#include "utils/ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

void ParticleGenerator::generateParticles(ParticleContainer &particles, const std::vector<ShapeInfo>& infovec) {
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

void ParticleGenerator::generateCuboid(ParticleContainer &particles, const ShapeInfo &info) {
    // reserve amount of particles we are going to create
    int total_count = info.N[0] * info.N[1] * info.N[2];
    particles.reserve(total_count);

    std::array<double, 3> currentPos{info.pos};

    for(int z = 0; z < info.N[2]; z++){
        for(int y = 0; y < info.N[1]; y++){
            for(int x = 0; x < info.N[0]; x++){
                Particle part{};
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

void ParticleGenerator::generateSphere(ParticleContainer &particles, const ShapeInfo &info) {
    // how many particles fit on the radius between center and edge
    int height = floor(info.radius / info.distance);
    // get parameters for cube generation
    std::vector<int> edges = {2*height + 1,2 * height + 1,2 * height + 1};
    std::array<double,3> cubeCorner = {info.pos[0]-(height*info.distance),
                                       info.pos[1]-(height*info.distance),
                                       info.pos[2]-(height*info.distance)};
    auto currentPos{cubeCorner};

    // ratio of volume of maximum inscribed sphere to cube is pi/6 ~= 0.523...
    int count = floor(pow((2*height+1),3) * 0.523598775598f);
    particles.reserve(count);

    for(int z = 0; z < edges[2]; z++){
        for(int y = 0; y < edges[1]; y++){
            for(int x = 0; x < edges[0]; x++){
                double delta_x = currentPos[0] - info.pos[0];
                double delta_y = currentPos[1] - info.pos[1];
                double delta_z = currentPos[2] - info.pos[2];
                double distance = std::abs(sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z));
                // only add if distance to center is less or equal the radius
                if(distance <= info.radius){
                    Particle part;
                    auto tempVel = info.vel + maxwellBoltzmannDistributedVelocity(info.brownianFactor, info.brownianDIM);
                    particles.emplace_back(Particle{currentPos, tempVel, info.mass});
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
    int height = floor(info.radius / info.distance);

    i.N = {2*height + 1,2 * height + 1,2 * height + 1};
    i.distance = info.distance;
    i.pos = {-(height*info.distance),
             -(height*info.distance),
             -(height*info.distance)};
    generateCuboid(particles, i);



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