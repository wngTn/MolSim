
#ifndef PSEMOLDYN_GROUPC_PHYSICSCALC_H
#define PSEMOLDYN_GROUPC_PHYSICSCALC_H

#include <vector>
#include "Particle.h"
#include "ParticleContainer.h"


class PhysicsCalc {
public:
    enum calctype{stoermerVerlet};

    PhysicsCalc() : delta_t(0.1), DIM(3){}
    PhysicsCalc(double delta_t, int DIM) : delta_t(delta_t), DIM(DIM){}
    PhysicsCalc(const PhysicsCalc&) = default;

    /**
     * @brief Calculates the forces between the particles
     */
    virtual void calcF(ParticleContainer& parts) = 0;

    /**
     * @brief Calculates the velocities of the particles
     */
    virtual void calcV(ParticleContainer& parts) = 0;

    /**
    * @brief Calculates the positions of the particles
    */
    virtual void calcX(ParticleContainer& parts) = 0;

    /**
     * @return string representation of calculator type
     */
     virtual std::string toString() = 0;

protected:
    const double delta_t;
    const int DIM;

};


#endif //PSEMOLDYN_GROUPC_PHYSICSCALC_H
