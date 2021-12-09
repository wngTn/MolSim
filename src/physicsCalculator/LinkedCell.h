#pragma once

#include <iostream>
#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/LennardJones.h"
#include "particleContainers/LinkedCellContainer.h"
#include "utils/ArrayUtils.h"
#include "spdlog/spdlog.h"

namespace calculator {
    class LinkedCell : public PhysicsCalc{

    public:
        LinkedCell(double sigma, double epsilon, double rCut) : sigma(sigma), epsilon(epsilon), rCut{rCut}{};

        std::string toString() override;

        /**
        * Returns the square of a number
        * @param x: the number
        */
        template<typename T> static
        T sqr(T x);

        /**
         *
         * @param grid
         * @param border 0 means outflow -> particle gets deleted; 1 means cyclic -> particle goes to the other side
         */
        static void moveParticles(LinkedCellContainer & grid);

        /**
         * override of the default calcX method, because here boundary conditions need to be applied
         * @param grid the particle container (here a LinkedCellContainer, will be downcasted)
         */
        void calcX(ParticleContainer & grid) const override;

        void calcF(ParticleContainer &grid) override;

        /**
         * Method explicitly for calculating the forces within a cell
         * @param cell
         */
        void calcFWithinCell(Cell & cell);

        /**
         * Generates a ghost particle on the other side of the border
         * @param grid the LinkedCellContainer
         * @param p the Particle that should be mirrored
         * @param bord the border it should be mirrored from
         * @return the ghost particle on the other side of the border
         * \note{This method is not as efficient as using a formula to calculate the force, thus not used}
         */
        static Particle generateGhostParticle(const LinkedCellContainer & grid, const Particle* p, int bord);

    private:

        inline void ljforce(Particle* p1, Particle* p2, double sqrd_dist) const;

        void reflectiveBoundary(LinkedCellContainer & grid, const std::array<int, 3> & currentIndexes) const;

        void calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> & neighbors,
                           Particle* p);

        double sigma = 1;
        double epsilon = 5;
        double rCut = 2.5 * epsilon;
    };

    template<typename T>
    T LinkedCell::sqr(T x) {
        return x * x;
    }

    void LinkedCell::ljforce(Particle* p1, Particle* p2, double sqrd_dist) const {

        double s = sqr(sigma) / sqrd_dist;
        s = s * s * s; // s = sqr(s) * s
        double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);

        auto force = f * (p2->getX() - p1->getX());

        // set old force
        // p1->setOldF(p1->getF());
        // p2->setOldF(p2->getF());

        p1->setF(p1->getF() + force);
        p2->setF(p2->getF() - force);
    }
}





