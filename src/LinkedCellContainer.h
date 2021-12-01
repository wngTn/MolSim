#pragma once

#include <vector>
#include <Particle.h>
#include "utils/ArrayUtils.h"
#include <iostream>
#include <cmath>
#include <iterator>

struct LinkedCellContainer {

    enum Border {outflow, cyclic};

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
         * @brief Provides the iterator for single particles at the start of the collection
         * @return iterator
         */
        std::vector<Particle>::iterator begin();

        /**
         * @brief Provides the iterator for single particles at the end of the collection
         * @return iterator
         */
        std::vector<Particle>::iterator end();

        /**
         * @brief Provides the _const_ iterator for single particles at the start of the collection
         * @return const iterator
         */
        [[nodiscard]] std::vector<Particle>::const_iterator begin() const;

        /**
         * @brief Provides the _const_ iterator for single particles at the end of the collection
         * @return const iterator
         */
        [[nodiscard]] std::vector<Particle>::const_iterator end() const;


        /**
         * @brief Moves an object to the collection, i.e. uses one of the constructor of the Particle class
         * and creates an object directly
         * @tparam Args Parameter pack
         * @param args The parameter for the constructor
         */
        template<typename... Args>
        void emplace_back(Args &&... args);

        /*
         * Removes particle at the position
         */
        std::vector<Particle>::iterator erase (std::vector<Particle>::const_iterator position);

        void toString();

    private:
        /**
         * The Particles in each cell
         */
        std::vector<Particle> particles;

    };

    /***********************************************************************/

    /**
     * Normal Constructor, which creates a grid with X*Y*Z elements
     * @param Xv length of X-Axis of the array == the length of the domain
     * @param Yv length of Y-Axis of the array == the length of the domain
     * @param Zv length of Z-Axis of the array == the length of the domain
     * @param rCutV the r_cut value
     * @param border the border type
     */
    LinkedCellContainer(int Xv, int Yv, int Zv, double rCutV, Border borderV = outflow) :
        grid{std::vector<LinkedCellContainer::Cell>(static_cast<int>(std::floor(Xv/rCutV))*
                                                            static_cast<int>(std::floor(Yv/rCutV))*
                                                            (static_cast<int>(std::floor(Zv/rCutV)) == 0 ? 1 :
                                                    static_cast<int>(std::floor(Zv/rCutV))))},
        dim{std::array<int, 3>{static_cast<int>(std::floor(Xv/rCutV)),
                               static_cast<int>(std::floor(Yv/rCutV)),
                               (static_cast<int>(std::floor(Zv/rCutV))) == 0 ? 1 :
                               static_cast<int>(std::floor(Zv/rCutV))}},
        lenDim{std::array<int, 3>{Xv, Yv, Zv}}, rCut{rCutV}, border{borderV} {};

    /**
     * Default constructor
     */
    LinkedCellContainer() = default;

    void setLenDim(const std::array<int, 3> &lenDim);

//    /**
//     * Get a list of all neighbors of a particle with index i
//     * @tparam i The index of the Cell
//     * @return A vector of Cells that neighbor the cell
//     */
//    template<int i>
//    [[nodiscard]] std::vector<Cell> getNeighbors() const;

    /***** Getters *****/

    [[nodiscard]] const std::vector<LinkedCellContainer::Cell> & getGrid() const;

    [[nodiscard]] double getRCut() const;

    [[nodiscard]] Border getBorder() const;

    [[nodiscard]] const std::array<int, 3> &getDim() const;

    [[nodiscard]] const std::array<int, 3> &getLenDim() const;


    /***** Setters *****/
    void setGrid(const std::vector<LinkedCellContainer::Cell> &grid);

    void setDim(const std::array<int, 3> &dim);

    void setRCut(double rCutV);

    void setBorder(Border border);

    std::vector<std::array<int, 3>> getNeighbors (const std::array<int, 3> & currentIndex) const;

    /**
     * Has X * Y * Z many elements
     */
    std::vector<LinkedCellContainer::Cell> grid;

 private:
    /**
     * The array describes the length of the respective dimensions
     * dim[0] = X, dim[1] = Y, dim[2] = Z
     */
    std::array<int, 3> dim{};
    /**
     * The array describes how long the domain of the respective dimensions should've been
     * lenDim[0] = X, lenDim[1] = Y, lenDim[2] = Z
     */
    std::array<int, 3>lenDim{};
    double rCut{};

    Border border{outflow};
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






