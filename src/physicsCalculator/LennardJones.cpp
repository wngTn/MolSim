#include "LennardJones.h"

namespace calculator {

    // atm just copied from StoermerVerlet, TODO check if correct
    void LennardJones::calcV(ParticleContainer &particles) {
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

    void LennardJones::calcF(ParticleContainer &particles) {
        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto[p1, p2] = *it;
            ljforce(p1, p2);
        }
    }

    // atm just copied from StoermerVerlet, TODO check if correct
    void LennardJones::calcX(ParticleContainer &particles) {
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

    void LennardJones::ljforce(Particle &p1, Particle &p2) {
        double sqrd_dist = 0;
        double force;
        for (int i = 0; i < DIM; i++) {
            sqrd_dist += LennardJones::sqr(p2.getX().at(i) - p1.getX().at(i));
        }
        double s = LennardJones::sqr(sigma) / sqrd_dist;
        s = s * s * s; // s = sqr(s) * s
        double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);
        for (int i = 0; i < DIM; i++) {
            force = f * (p2.getX().at(i) - p1.getX().at(i));
            p1.setF(i, p1.getF().at(i) + force);
            p2.setF(i, p2.getF().at(i) - force);
        }
    }

}