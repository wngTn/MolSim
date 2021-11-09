#include "LennardJones.h"

namespace calculator {

    void LennardJones::calcF(ParticleContainer &particles) {
        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto[p1, p2] = *it;
            ljforce(p1, p2);
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