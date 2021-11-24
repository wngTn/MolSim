#pragma once

#include <vector>
#include <Particle.h>
#include "utils/ArrayUtils.h"
#include <iostream>

struct LinkedCellContainer {
    /**
     * Each element in the grid is a cell
     */
    struct Cell {
        /**
         * Normal default constructor
         */
        Cell() : particles{std::vector<Particle>{}} {};

        void setParticles(const std::vector<Particle> &particlesV) {
            Cell::particles = particlesV;
        }

        [[nodiscard]] const std::vector<Particle> &getParticles() const {
            return particles;
        }

        /**
         * @brief Moves an object to the collection, i.e. uses one of the constructor of the Particle class
         * and creates an object directly
         * @tparam Args Parameter pack
         * @param args The parameter for the constructor
         */
        template<typename... Args>
        void emplace_back(Args &&... args);


     private:
        /**
         * The Particles in each cell
         */
        std::vector<Particle> particles;

    };

    /***********************************************************************/

    /**
     * Normal Constructor, which creates a grid with X*Y*Z elements
     * @param Xv length of X-Axis of the array (not the length of the domain)
     * @param Yv length of Y-Axis of the array (not the length of the domain)
     * @param Zv length of Z-Axis of the array (not the length of the domain)
     * @param rCutV the r_cut value
     */
    LinkedCellContainer(int Xv, int Yv, int Zv, double rCutV) : grid{std::vector<LinkedCellContainer::Cell>(Xv*Yv*Zv)}, X{Xv},
        Y{Yv}, Z{Zv}, rCut{rCutV} {};

    /**
     * Default constructor
     */
    LinkedCellContainer() = default;

//    /**
//     * Get a list of all neighbors of a particle with index i
//     * @tparam i The index of the Cell
//     * @return A vector of Cells that neighbor the cell
//     */
//    template<int i>
//    [[nodiscard]] std::vector<Cell> getNeighbors() const;

    /**
     * Adds a particle to the grid in the cell with the index i
     * @param particle the particle that should be added
     * @param i the index of the cell
     */
    void addParticle(const Particle & particle, int i);

    /**
     * Adds a particle list to the grid in the cell with the index i
     * @param particles the particles that should be added
     * @param i the index of the cell
     */
    void addParticle(const std::vector<Particle> & particles, int i);

    /***** Getters *****/
    [[nodiscard]] const std::vector<LinkedCellContainer::Cell> &getGrid() const;

    [[nodiscard]] int getX() const;

    [[nodiscard]] int getY() const;

    [[nodiscard]] int getZ() const;

    [[nodiscard]] double getRCut() const;


    /***** Setters *****/
    void setGrid(const std::vector<LinkedCellContainer::Cell> &grid);

    void setX(int x);

    void setY(int y);

    void setZ(int z);

    void setRCut(double rCutV);

 private:
    std::vector<LinkedCellContainer::Cell> grid;
    int X{}, Y{}, Z{};
    double rCut{};
};


//template<int i>
//std::vector<LinkedCellContainer::Cell> LinkedCellContainer::getNeighbors() const {
//    std::vector<Cell> neighbors{};
//    std::vector<int> neighborIndexes;
//    neighborIndexes.reserve((Z == 1) ? 8 : 26);
//
//    if (Z == 1) {
//        neighborIndexes = {/* Only one layer in 2 Dimensions*/
//                -X-1, -X, -X+1,
//                -1, 1,
//                X-1, X, X+1};
//    } else {
//        neighborIndexes = {/* The upper layer*/
//                (X * Y - 1) - X, (X * Y) - X, (X * Y + 1) - X,
//                -1 - X, -X, 1 - X,
//                -(X * Y) - 1 - X, -(X * Y) - X, -(X * Y) + 1 - X,
//                /* The middle layer */
//                (X * Y - 1), (X * Y), (X * Y + 1),
//                -1, 1,
//                -(X * Y) - 1, -(X * Y), -(X * Y) + 1,
//                /* The lower layer */
//                (X * Y - 1) + X, (X * Y) + X, (X * Y + 1) + X,
//                -1 + X, X, 1 + X,
//                -(X * Y) - 1 + X, -(X * Y) + X, -(X * Y) + 1 + X};
//    }
//
//    for (int &neighborIndex_e: neighborIndexes) {
//        neighborIndex_e += i;
//    }
//
//    for (int j: neighborIndexes) {
//        if (j >= 0 && j < (X * Y * Z)) {
//            neighbors.template emplace_back(grid[j]);
//        }
//    }
//    return neighbors;
//}

template<typename... Args>
void LinkedCellContainer::Cell::emplace_back(Args &&... args) {
    particles.emplace_back(std::forward<Args>(args)...);
}





