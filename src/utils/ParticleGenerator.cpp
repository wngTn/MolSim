#include "ParticleGenerator.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

bool ParticleGenerator::useBrownianMotion;
std::array<double, 3> ParticleGenerator::pos;
double ParticleGenerator::distance;
double ParticleGenerator::mass;
std::array<double, 3> ParticleGenerator::vel;
double ParticleGenerator::brownianMotionMean;
std::array<int,3> ParticleGenerator::noParts;
double ParticleGenerator::radius;

void ParticleGenerator::generateParticles(ParticleContainer &particles, const std::string &file, bool brownianM) {
    geometric_type type = readFile(file);
    useBrownianMotion = brownianM;
    switch (type){
        case cuboid:
            generateCuboid(particles);
            break;
        case sphere:
            generateSphere(particles);
            break;
    }
}

// this is not very nice and could/should be replaced by e.g. a good JSON parser
ParticleGenerator::geometric_type ParticleGenerator::readFile(const std::string &file) {
    std::ifstream input_file(file);
    std::string tmp_string;
    ParticleGenerator::geometric_type type;
    if(input_file.is_open()){
        getline(input_file, tmp_string);
        // read input specific to one form
        if(tmp_string == "CUBOID"){
            type = cuboid;
            int n1, n2, n3;
            input_file >> n1 >> n2 >> n3;
            // std::cout << "N: " << n1 << ", " << n2 << ", " << n3 << std::endl;
            noParts = std::array<int, 3>{n1,n2,n3};
        }else if(tmp_string == "SPHERE"){
            type = sphere;
            input_file >> radius;
        }else{
            std::cout << "Error: Invalid generator file format" << std::endl;
            exit(-1);
            // TODO Fehlerbehandlung
        }
        // read input all forms have
        double x,y,z,v1,v2,v3;
        input_file >> x >> y >> z >> v1 >> v2 >> v3 >> distance >> mass >> brownianMotionMean;
        // std::cout << "pos and v" << x << ", " << y << ", "<< z << ", "<< v1 << ", "<< v2 << ", " << v3 << std::endl;
        // std::cout << "d,m,brown" << distance << ", " << mass << ", " << brownianMotionMean << std::endl;
        pos = std::array<double,3>{x,y,z};
        vel = std::array<double,3>{v1,v2,v3};

        return type;

    } else {
        std::cout << "Error: could not open file " << file << std::endl;
        exit(-1);
    }

}

void ParticleGenerator::generateCuboid(ParticleContainer &particles) {
    // reserve amount of particles we are going to create
    int total_count = noParts[0] * noParts[1] * noParts[2];
    particles.reserve(total_count);

    std::array<double, 3> currentPos{pos};

    for(int z = 0; z < noParts[2]; z++){
        for(int y = 0; y < noParts[1]; y++){
            for(int x = 0; x < noParts[0]; x++){
                Particle part;
                if(useBrownianMotion){
                    // TODO check if this is correct
                    auto tempVel = vel;
                    auto brownMot = maxwellBoltzmannDistributedVelocity(brownianMotionMean, 3);
                    // auto oldV = part.getV();
                    for(int i = 0; i < 3; i++){
                        tempVel[i] += brownMot[i];
                        // part.setV(i,oldV[i]+brownMot[i]);
                    }
                    part = Particle{currentPos, tempVel, mass};
                }else{
                    part = Particle{currentPos, vel, mass};
                }
                particles.emplace_back(part);
                currentPos[0] += distance;
            }
            currentPos[0] = pos[0];
            currentPos[1] += distance;
        }
        currentPos[0] = pos[0];
        currentPos[1] = pos[1];
        currentPos[2] += distance;
    }
}

// TODO
void ParticleGenerator::generateSphere(ParticleContainer &particles) {
    // dummy particle so anything shows up
    particles.emplace_back(Particle{pos, vel, mass});
}
