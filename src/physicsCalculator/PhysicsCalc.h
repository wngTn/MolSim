#pragma once

#include <vector>
#include "Particle.h"
#include "particleContainers/ParticleContainer.h"


class PhysicsCalc {
public:
    enum calctype {
        gravitation,
        lennardJones,
        unknown
    };

    /**
     * @brief Constructor
     */
    PhysicsCalc() = default;

    /**
     * @brief Constructor with arguments
     * @param delta_t the given delta
     * @param DIM the dimension we are in
     * @param g gravitational force
     */
    PhysicsCalc(double delta_t, int DIM) : delta_t(delta_t), DIM(DIM) {}

    PhysicsCalc(const PhysicsCalc &) = default;

    /**
     * @brief Calculates the forces between the particles according to the specification
     * @param particles the container with the particles
     */
    virtual void calcF(ParticleContainer &particles) = 0;

    [[nodiscard]] double getDeltaT() const;


    virtual std::string toString() = 0;

    // since calcV and calcX are the same for all calc methods, they are not virtual and implemented in PhysicsCalc.cpp

    /**
     * @brief Calculates the velocities of the particles
     * @param particles the container with the particles
     */
    virtual void calcX(ParticleContainer &particles) const;

    /**
     * @brief Calculates the positions of the particles
     * @param particles the container with the particles
     */
    virtual void calcV(ParticleContainer &particles) const;

    void setDeltaT(double deltaT);

    [[nodiscard]] int getDim() const;

    void setDim(int dim);

protected:
    double delta_t{0.1};

    int DIM{3};

};
