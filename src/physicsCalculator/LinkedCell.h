#pragma once

#include "physicsCalculator/PhysicsCalc.h"
#include "LinkedCellContainer.h"

namespace calculator {
    class LinkedCell : public PhysicsCalc {

    public:
        LinkedCell(double sigma, double epsilon) : sigma(sigma), epsilon(epsilon) {};

        void calcF_LC(LinkedCellContainer &particles);

        std::string toString() override;

        /**
        * Returns the square of a number
        * @param x: the number
        */
        template<typename T>
        T sqr(T x);

    private:
        /**
         * Calculates the Lennard-Jones potential between to particles
         * @param p1 The first particle
         * @param p2 The second particle
         */
        void ljforce(Particle &p1, Particle &p2);

        double sigma = 1;
        double epsilon = 5;

    };

    template<typename T>
    T LinkedCell::sqr(T x) {
        return x * x;
    }
}



