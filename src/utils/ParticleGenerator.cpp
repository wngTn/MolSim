#include "ParticleGenerator.h"
#include "utils/ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <typeinfo>

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
    // load json from file to json object
    input_file >> j;

    for (auto &shape : j["shapes"]){
        std::cout << "ELEM: " << shape << std::endl;
        std::string type = shape["type"];
        ShapeInfo i{};
        if(type == "cuboid"){
            i.type = ParticleGenerator::cuboid;
            i.pos = shape["pos"];
            i.vel = shape["vel"];
            i.distance = shape["distance"];
            i.mass = shape["mass"];
            i.brownianFactor = shape["brownianFactor"];
            i.brownianDIM = shape["brownianDIM"];

            i.N = shape["N"];
        }else if(type == "sphere"){
            i.type = ParticleGenerator::cuboid;
            i.pos = shape["pos"];
            i.vel = shape["vel"];
            i.distance = shape["distance"];
            i.mass = shape["mass"];
            i.brownianFactor = shape["brownianFactor"];
            i.brownianDIM = shape["brownianDIM"];

            i.radius = shape["radius"];
        }else{
            std::cerr << "Invalid Shape type" << std::endl;
            continue;
        }
        // add parsed sphere to vector
        vec.emplace_back(i);
    }

    return vec;
}

// this is not very nice and could/should be replaced by e.g. a good JSON parser
ParticleGenerator::ShapeInfo ParticleGenerator::readFile(const std::string &file) {
    ShapeInfo info{};

    std::ifstream input_file(file);
    std::string tmp_string;

    if(input_file.is_open()){
        getline(input_file, tmp_string);
        // read input specific to one form
        if(tmp_string == "CUBOID"){
            info.type = cuboid;
            int n1, n2, n3;
            input_file >> n1 >> n2 >> n3;
            // std::cout << "N: " << n1 << ", " << n2 << ", " << n3 << std::endl;
            info.N = std::array<int, 3>{n1,n2,n3};
        }else if(tmp_string == "SPHERE"){
            info.type = sphere;
            input_file >> info.radius;
        }else{
            std::cout << "Error: Invalid generator file format" << std::endl;
            exit(-1);
            // TODO Fehlerbehandlung
        }
        // read input all forms have
        double x,y,z,v1,v2,v3;
        input_file >> x >> y >> z >> v1 >> v2 >> v3 >> info.distance >> info.mass >> info.brownianFactor >> info.brownianDIM;
        std::cout << "pos and v: " << x << ", " << y << ", "<< z << ", "<< v1 << ", "<< v2 << ", " << v3 << std::endl;
        std::cout << "d,m,brown: " << info.distance << ", " << info.mass << ", " << info.brownianFactor << std::endl;
        info.pos = std::array<double,3>{x,y,z};
        info.vel = std::array<double,3>{v1,v2,v3};

        return info;

    } else {
        std::cout << "Error: could not open file " << file << std::endl;
        exit(-1);
    }

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

// JSON format for when the above garbage is replaced
/* {
   "shapes":[
            {
                "type": "cuboid"
                "x": 1.0,
                "y": 1.0,
                "z": 1.0,
                "v1": 1.0,
                "v2": 1.0,
                "v3": 1.0,
                "n1": 1.0,
                "n2": 1.0,
                "n3": 1.0,
                "distance": 1.0,
                "mass": 1.0,
                "brownianFactor": 1.0
            },
            {   "type": "sphere"
                "x": 1.0,
                "y": 1.0,
                "z": 1.0,
                "v1": 1.0,
                "v2": 1.0,
                "v3": 1.0,
                "radius": 1.0,
                "distance": 1.0,
                "mass": 1.0,
                "brownianFactor": 1.0
            },
            {
                // potentially more shapes
            }
   ]
}
*/