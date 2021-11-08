#include "StoermerVerlet.h"

namespace calculator {

    void StoermerVerlet::calcF(ParticleContainer &particles) {
        for (auto &p: particles) {
            for (int i = 0; i < DIM; ++i) {
                p.setF(i, 0);
            }
        }

        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto[p1, p2] = *it;
            grav_force(p1, p2);
        }

    }

    void StoermerVerlet::calcX(ParticleContainer &particles) {
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


    void StoermerVerlet::calcV(ParticleContainer &particles) {
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

    void StoermerVerlet::grav_force(Particle &p1, Particle &p2) {
        double sqrd_dist = 0;
        double force;

        // calculate the squared distance
        for (int i = 0; i < DIM; ++i) {
            sqrd_dist += StoermerVerlet::sqr(p2.getX().at(i) - p1.getX().at(i));
        }
        // left side of the term
        double var = p1.getM() * p2.getM() / (sqrt(sqrd_dist) * sqrd_dist);
        // multiplying with (p2 - p1) and setting the force
        for (int i = 0; i < DIM; ++i) {
            force = (var * (p2.getX()[i] - p1.getX()[i]));
            p1.setF(i, p1.getF()[i] + force);
            p2.setF(i, p2.getF()[i]- force);
        }
    }
}