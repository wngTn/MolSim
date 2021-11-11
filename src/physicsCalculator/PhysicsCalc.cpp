#include "PhysicsCalc.h"
#include "utils/ArrayUtils.h"

void PhysicsCalc::calcV(ParticleContainer &particles) const {
    // calculate new velocities
    for (auto &p: particles) { // loop over every particle
        // go through all three dimensions

        auto newV = p.getV() + delta_t * 0.5 / p.getM() * (p.getF() + p.getOldF());
        p.setV(newV);
        /* for (int i = 0; i < DIM; ++i) {
            // calculates new velocity
            new_V = p.getV().at(i) + (delta_t * 0.5 / p.getM() * (p.getF().at(i) + p.getOldF().at(i)));
            p.setV(i, new_V);
        } */
    }
}

void PhysicsCalc::calcX(ParticleContainer &particles) const {
    // calculate new positions
    for (auto &p: particles) { // loop over every particle
        // go through all three dimensions

        auto newX = p.getX() + delta_t * ( p.getV() + delta_t * 0.5 / p.getM() * p.getF());
        p.setX(newX);
        /* for (int i = 0; i < DIM; ++i) {
            // calculates new position
            new_X = p.getX().at(i) + (delta_t * (p.getV().at(i) + delta_t * .5 / p.getM() * p.getF().at(i)));
            // set new position
            p.setX(i, new_X);
        } */

    }
}