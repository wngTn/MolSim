
#ifndef PSEMOLDYN_GROUPC_PHYSICSCALC_H
#define PSEMOLDYN_GROUPC_PHYSICSCALC_H

#include <vector>
#include "Particle.h"
#include "ParticleContainer.h"


class PhysicsCalc {
public:
    PhysicsCalc() : delta_t(0.1), DIM(3){}
    PhysicsCalc(double delta_t, int DIM) : delta_t(delta_t), DIM(DIM){}
    PhysicsCalc(const PhysicsCalc&) = default;


    virtual void calcF(ParticleContainer& parts) = 0;
    virtual void calcV(ParticleContainer& parts) = 0;
    virtual void calcX(ParticleContainer& parts) = 0;

protected:
    const double delta_t;
    const int DIM;

};


#endif //PSEMOLDYN_GROUPC_PHYSICSCALC_H
