#pragma once

#include "physicsCalculator/PhysicsCalc.h"
#include "ParticleContainer.h"

namespace calculator {

    class LennardJones : public PhysicsCalc {

    public:
        LennardJones(double sigma, double epsilon) : sigma(sigma), epsilon(epsilon) {};

        void calcF(ParticleContainer &particles) override;

        std::string toString() override;
        /**
        * Returns the square of a number
        * @param x: the number
        */
        template<typename T>
        T sqr(T x);

    private:
        void ljforce(Particle &p1, Particle &p2);

        double sigma = 1;
        double epsilon = 5;

    };

    template<typename T>
    T LennardJones::sqr(T x) {
        return x * x;
    }

}

