#pragma once

#include "physicsCalculator/PhysicsCalc.h"
#include "ParticleContainer.h"

namespace calculator {

    class StoermerVerlet : public PhysicsCalc {


        void calcF(ParticleContainer &particles) override;

        /**
        * Returns the square of a number
        * @param x: the number
        */
        template<typename T>
        T sqr(T x);

    private:

        /** Computes the gravitational force between two particles
       *
       * @param p1 first particle
       * @param p2 second particle
       */
        void grav_force(Particle &p1, Particle &p2);


    };

    template<typename T>
    T StoermerVerlet::sqr(T x) {
        return x * x;
    }

}
