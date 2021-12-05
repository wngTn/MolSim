#include "Gravitation.h"
#include "utils/ArrayUtils.h"

namespace calculator {

    void Gravitation::calcF(ParticleContainer &container) {
        auto& particles = static_cast<DirectSumParticleContainer&>(container);
        for (auto &p: particles) {
            p.setOldF(p.getF());

            p.setF(std::array<double,3>{0.,0.,0.});

        }

        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto[p1, p2] = *it;
            grav_force(p1, p2);
        }

    }

    void Gravitation::grav_force(Particle &p1, Particle &p2) {
        double sqrd_dist = 0;
        // calculate the squared distance
        for (int i = 0; i < DIM; ++i) {
            sqrd_dist += Gravitation::sqr(p2.getX()[i] - p1.getX()[i]);
        }
        // left side of the term
        double var = p1.getM() * p2.getM() / (sqrt(sqrd_dist) * sqrd_dist);
        // multiplying with (p2 - p1) and setting the force
        auto force = var * (p2.getX() - p1.getX());
        p1.setF(p1.getF() + force);
        p2.setF(p2.getF() - force);
    }

    std::string Gravitation::toString(){
        return "Gravitation";
    }
}