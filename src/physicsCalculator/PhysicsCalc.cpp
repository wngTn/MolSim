#include "PhysicsCalc.h"

void PhysicsCalc::calcV(ParticleContainer &particles) const {
    double new_V;
    // calculate new velocities
    for (auto &p: particles) { // loop over every particle
        // go through all three dimensions
        for (int i = 0; i < DIM; ++i) {
            // calculates new velocity
            new_V = p.getV().at(i) + (delta_t * 0.5 / p.getM() * (p.getF().at(i) + p.getOldF().at(i)));
            p.setV(i, new_V);
        }
    }
}

void PhysicsCalc::calcX(ParticleContainer &particles) const {
    double new_X;
    // calculate new positions
    for (auto &p: particles) { // loop over every particle
        // go through all three dimensions
        for (int i = 0; i < DIM; ++i) {
            // calculates new position
            new_X = p.getX().at(i) + (delta_t * (p.getV().at(i) + delta_t * .5 / p.getM() * p.getF().at(i)));
            // set new position
            p.setX(i, new_X);
        }
        // set old force
        p.setOldF(p.getF());
    }
}