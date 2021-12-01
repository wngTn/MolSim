#pragma once

#include "particleContainers/DirectSumParticleContainer.h"

class ParticleGenerator {
public:
    // TODO adapt to Linked cell Container
    static void generateParticles(DirectSumParticleContainer &particles, const std::string &file);

    enum geometric_type{
        cuboid,
        sphere
    };


    struct ShapeInfo {
        geometric_type type;
        // for sphere AND cuboid
        // C: lower left front side corner, S: center
        std::array<double,3> pos;
        // initial velocity of the particles
        std::array<double,3> vel;
        // mass of particles
        double mass;
        // distance between particles
        double distance;
        // mean velocity of brownian motion
        double brownianFactor;
        int brownianDIM;
        // only used for sphere
        double radius;
        // only used for cuboid
        std::array<int,3> N;
    };

private:




    /**
     * parses a JSON file and returns the content (if well formed) as a vector of ShapeInfo object
     * @param file the path to the input file
     * @return a vector containing ShapeInfo Objects for all shapes that should be generated
     */
    static std::vector<ShapeInfo> readJSON(const std::string &file);

    /**
     * generates a cuboid specified by the passed parameters
     * @param particles the ParticleContainer the created cube is stored in
     * @param info the ShapeInfo struct containing the parameters
     */
    static void generateCuboid(DirectSumParticleContainer &particles, const ShapeInfo &info);


    // this creates a cube and cuts the sphere out of it. not very cool sphere but min distance is kept
    /**
     * generates a sphere specified by the passed parameters
     * @param particles the ParticleContainer the created sphere is stored in
     * @param info the ShapeInfo struct containing the parameters
     */
    static void generateSphere(DirectSumParticleContainer &particles, const ShapeInfo &info);

    // currently not used, testing other methods for sphere generation
    // this produces veeeery smooth & nice spheres, but the minimum distance is not kept, so not suited for LJP
    // also not optimized at all bc still WIP
    static void generateSphere2(DirectSumParticleContainer &particles, const ShapeInfo & info);

};

