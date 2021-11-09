#pragma once

#include "ParticleContainer.h"

class ParticleGenerator {
public:
    static void generateParticles(ParticleContainer &particles, const std::string &file);

private:

    enum geometric_type{
        cuboid,
        sphere
    };

    /**
     * reads an input file and sets the static variables accordingly
     * @param file the filename of the input file
     * @return the type of the geometric shape to generate
     */
     static ParticleGenerator::geometric_type readFile(const std::string &file) ;

    /**
     * generates a cuboid specified by the passed parameters
     * @param particles the ParticleContainer the created cube is stored in
     */
    static void generateCuboid(ParticleContainer &particles);

    /**
     * generates a sphere specified by the passed parameters
     * @param particles the ParticleContainer the created sphere is stored in
     */
    static void generateSphere(ParticleContainer &particles);

    // for sphere AND cuboid
    // C: lower left front side corner, S: center
    static std::array<double, 3> pos;
    // distance between particles
    static double distance;
    // mass of particles
    static double mass;
    // initial velocity of the particles
    static std::array<double, 3> vel;
    // mean velocity of brownian motion
    static double brownianMotionMean;

    // for cuboid only
    // the number of particles per dimension (N1 x N2 x N3)
    static std::array<int,3> noParts;

    // for sphere only
    // radius
    static double radius;
};

