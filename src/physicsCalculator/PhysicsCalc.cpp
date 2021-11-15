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
            new_V = p_result.getV().at(i) + (delta_t * 0.5 / p_result.getM() * (p_result.getF().at(i) + p_result.getOldF().at(i)));
            p_result.setV(i, new_V);
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
            new_X = p_result.getX().at(i) + (delta_t * (p_result.getV().at(i) + delta_t * .5 / p_result.getM() * p_result.getF().at(i)));
            // set new position
            p_result.setX(i, new_X);
        } */

    }
}