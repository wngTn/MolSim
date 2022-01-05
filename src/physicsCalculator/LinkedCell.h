#pragma once

#include <iostream>
#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/LennardJones.h"
#include "particleContainers/LinkedCellContainer.h"
#include "utils/ArrayUtils.h"
#include "spdlog/spdlog.h"

#include "nlohmann/json.hpp"

namespace calculator {
    class LinkedCell : public PhysicsCalc{

    public:
        LinkedCell(double sigma, double epsilon, double rCut, bool smoothed=false, double rl = 0.0) : sigma(sigma), epsilon(epsilon),
                                                                                                      rCut{rCut}, smoothed(smoothed), rl(rl){
            epsilonTable = {{epsilon}};
            sigmaTable = {{sigma}};
        };

        std::string toString() override;

        /**
        * Returns the square of a number
        * @param x: the number
        */
        template<typename T> static
        T sqr(T x);

        /**
         * This method simply sets all Particles to invalid, if they cross any borders,
         * or puts them on the other side, if there are cyclic borders
         * @param grid the grid
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


        void setSigmaTable(const std::vector<std::vector<double>> &sigmaTable);

        void setEpsilonTable(const std::vector<std::vector<double>> &epsilonTable);

        void setMapping(std::vector<std::pair<int, std::pair<double, double>>>& mapping);

        void setRZero(double rZ);

        void setStiffnessConstant(double stiffnessConstant);

        void setMembrane(bool mem);

    private:

        inline void ljforce(Particle* p1, Particle* p2, double sqrd_dist) const;

        inline void ljforce_smoothed(Particle* p1, Particle* p2, double sqrd_dist) const;

        void harmonic_potential(Particle* p1, Particle* p2, double sqrd_dist) const;

        void reflectiveBoundary(LinkedCellContainer & grid, const std::array<int, 3> & currentIndexes) const;

        void calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> & neighbors,
                           Particle* p);

        /**
         * Calculates the forces between p and its neighbor for the periodic boundary
         * The bord tells this method how to calculate the distance
         * @param grid the whole container
         * @param neighbors the neighbor index
         * @param p the particle we are calculating forces on
         * @param mirror the array we should add so we get the mirrored position
         */
        void calcPerNeighbors(LinkedCellContainer &grid, const std::array<int, 3> & neighbors,
                              Particle* p, const std::array<double, 3> & mirror) const;

        static bool gridNeighbors(std::array<int,3> index1, std::array<int,3> index2);

        double sigma = 1;

        double epsilon = 5;
        double rCut = 2.5 * epsilon;
        // average bond length of molecule pair, used for membrane
        double rZero{};
        // used for membrane
        double stiffnessConstant{};

        // whether any membranes exist, used only to shortcut some checks
        bool membrane = false;

        // whether smoothed LJ is used
        bool smoothed;

        // r_l used for smoothed LJ
        double rl;

        std::vector<std::vector<double>> sigmaTable = {{0.0}};
        std::vector<std::vector<double>> epsilonTable = {{0.0}};
        std::vector<std::pair<int, std::pair<double, double>>> mapping = {};

        using json = nlohmann::json;

        friend void to_json(json&, const LinkedCell&);

        friend void from_json(const json&, LinkedCell& p);

    };

    template<typename T>
    T LinkedCell::sqr(T x) {
        return x * x;
    }

    void LinkedCell::ljforce(Particle* p1, Particle* p2, double sqrd_dist) const {
        //double s = sqr(sigma) / sqrd_dist;
        double s = sqr(sigmaTable[p1->getSEIndex()][p2->getSEIndex()]) / sqrd_dist;
        s = s * s * s; // s = sqr(s) * s
        //  double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);
        double f = 24 * epsilonTable[p1->getSEIndex()][p2->getSEIndex()] * s / sqrd_dist * (1 - 2 * s);

        auto force = f * (p2->getX() - p1->getX());

        // set old force
        // p1->setOldF(p1->getF());
        // p2->setOldF(p2->getF());
        //if(force[0] > 500 || force[1] > 500 || force[2] > 500){
            //std::cout << "calculated force with combined σ " << sigmaTable[p1->getSEIndex()][p2->getSEIndex()] << " and combined ε "
            //<< epsilonTable[p1->getSEIndex()][p2->getSEIndex()] << std::endl;
        //}
        p1->setF(p1->getF() + force);
        p2->setF(p2->getF() - force);
    }

    void LinkedCell::ljforce_smoothed(Particle *p1, Particle *p2, double sqrd_dist) const {
        std::cout << "smoothed cald\n";
        double dist = sqrt(sqrd_dist);
        if(dist <= rl){
            ljforce(p1,p2,sqrd_dist);
            return;
        }else{ // dist < rCut checked b4 calling function
            double sigma_pow_6 = std::pow(sigmaTable[p1->getSEIndex()][p2->getSEIndex()], 6);
            double dist_pow_6 = sqrd_dist * sqrd_dist * sqrd_dist;
            auto first_part = (rCut - dist) * (- (24*sigma_pow_6)*epsilonTable[p1->getSEIndex()][p2->getSEIndex()])/(dist_pow_6 * dist_pow_6 * sqrd_dist * std::pow(rCut-rl,3));
            double second_part = rCut * rCut * (2*sigma_pow_6 - dist_pow_6) + rCut * (3*rl - dist)*(dist_pow_6 - 2 * sigma_pow_6) +
                                 dist * (5*rl*sigma_pow_6 - 2 * rl * dist_pow_6 - 3 * sigma_pow_6*dist + dist_pow_6*dist);
            auto force = (first_part * second_part) * (p2->getX() - p1->getX());
            p1->setF(p1->getF() + force);
            p2->setF(p2->getF() - force);
        }
    }

    constexpr double SIXTH_ROOT_OF_TWO = 1.12246204830937298;
    constexpr double SQR_ROOT_OF_TWO = 1.4142135623730950488;
}





