#pragma once

#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/LennardJones.h"
#include "LinkedCellContainer.h"
#include "utils/ArrayUtils.h"

namespace calculator {
    class LinkedCell : public PhysicsCalc{

    public:
        LinkedCell(double sigma, double epsilon, double rCut, double delta_t) : sigma(sigma), epsilon(epsilon), rCut{rCut} ,delta_t{delta_t}{};

        void calcF_LC(LinkedCellContainer &grid);

        void calcF(ParticleContainer &grid) override;

        std::string toString() override;

        /**
        * Returns the square of a number
        * @param x: the number
        */
        template<typename T> static
        T sqr(T x);

        /**
         * Returns the index in the 1D Grid container
         * @param currentIndexes the current indexes in the three dimensions
         * @param X Length of X axis
         * @param Y Length of Y axis
         * @param Z Length of Z axis
         * @return The index
         */
        static inline int index(const std::array<int , 3> & currentIndexes, const std::array<int, 3> & dim);

        static void moveParticles(LinkedCellContainer & grid);

        void calcX(ParticleContainer & grid) const override;

        void calcXcell(LinkedCellContainer::Cell & cell) const;

        void calcV(ParticleContainer & grid) const override;

        void calcVcell(LinkedCellContainer::Cell & cell) const;

    private:

        inline void ljforce(Particle &p1, Particle &p2, double sqrd_dist);

        void skipOutOfBoundary(LinkedCellContainer & grid, const std::array<int, 3> & neighbors,
                               Particle & p);

        void calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> & neighbors,
                           Particle &p);

        double sigma = 1;
        double epsilon = 5;
        double rCut = 2.5 * epsilon;
        double delta_t{0.1};
    };


    int LinkedCell::index(const std::array<int, 3> &currentIndexes, const std::array<int, 3> & dim) {
        return currentIndexes[0] + dim[0] * (currentIndexes[1] + dim[1] * currentIndexes[2]);
    }

    template<typename T>
    T LinkedCell::sqr(T x) {
        return x * x;
    }

    void LinkedCell::ljforce(Particle &p1, Particle &p2, double sqrd_dist) {
        double s = sqr(sigma) / sqrd_dist;
        s = s * s * s; // s = sqr(s) * s
        double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);

        auto force = f * (p2.getX() - p1.getX());

        // set old force
        p1.setOldF(p1.getF());
        // p2.setOldF(p2.getF());

        p1.setF(p1.getF() + force);
        // p2.setF(p2.getF() - force);
        std::cout<<"LINKED LIST: Particle with Type: "<<p1.getType()<<" after Force Calc with Particle Type: "<<
                 p2.getType()<<" is: ("<<p1.getF()[0]<<", "<<p1.getF()[1]<<", "<<p1.getF()[2]<<")"<<std::endl;
    }
}





