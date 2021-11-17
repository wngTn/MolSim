#include "LennardJones.h"
#include "utils/ArrayUtils.h"


namespace calculator {

    void LennardJones::calcF(ParticleContainer &particles) {
        for(auto &p : particles){
            p.setF({0.,0.,0.});
        }

        for (auto it = particles.pair_begin(); it != particles.pair_end(); ++it) {
            auto [p1, p2] = *it;
            ljforce(p1, p2);
        }
        // this (sadly) is a little faster, but as performance is not extremely critical atm we use the cooler version
        // when requiring optimal performance this has to be replaced,
        // but currently wasting the time spent on the PairIterator makes me too sad
        /* for(auto it = particles.begin(); it != particles.end(); ++it){
            for(auto it2 = it; it2 != particles.end(); ++it2){
                ljforce(*it,*it2);
            }
        } */
    }

    void LennardJones::ljforce(Particle &p1, Particle &p2) {
        double sqrd_dist = 0;
        for (int i = 0; i < DIM; i++) {
            sqrd_dist += LennardJones::sqr(p2.getX()[i] - p1.getX()[i]);
        }
        double s = LennardJones::sqr(sigma) / sqrd_dist;
        s = s * s * s; // s = sqr(s) * s
        double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);

        auto force = f * (p2.getX() - p1.getX());

        // set old force
        p1.setOldF(p1.getF());
        p2.setOldF(p2.getF());

        p1.setF(p1.getF() + force);
        p2.setF(p2.getF() - force);
    }

    std::string LennardJones::toString(){
        return "LennardJones";
    }
}