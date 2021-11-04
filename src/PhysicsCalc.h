#pragma once

#include <vector>
#include "Particle.h"
#include "ParticleContainer.h"


class PhysicsCalc {
public:
    enum calctype{stoermerVerlet};

    /**
     * @brief Constructor
     */
    PhysicsCalc()= default;
    /**
     * @brief Constructor with arguments
     * @param delta_t the given delta
     * @param DIM the dimension we are in
     */
    PhysicsCalc(double delta_t, int DIM) : delta_t(delta_t), DIM(DIM){}

    PhysicsCalc(const PhysicsCalc&) = default;

     /**
      * @brief Calculates the forces between the particles
      * @param particles the container with the particles
      */
    virtual void calcF(ParticleContainer& particles) = 0;

    /**
     * @brief Calculates the velocities of the particles
     * @param particles the container with the particles
     */
    virtual void calcV(ParticleContainer& particles) = 0;

    /**
    * @brief Calculates the positions of the particles
     * @param particles the container with the particles
    */
    virtual void calcX(ParticleContainer& particles) = 0;

    void setDeltaT(double dt){
        delta_t = dt;
    }

    void setDim(int d){
        DIM = d;
    }

protected:
    double delta_t{0.1};
    int DIM{3};

};
