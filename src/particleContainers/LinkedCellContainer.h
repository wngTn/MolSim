#pragma once

#include <vector>
#include <cmath>
#include <functional>

#include "Particle.h"
#include "ParticleContainer.h"
#include "Cell.h"

class LinkedCellContainer : public ParticleContainer{
public:

    enum Border {outflow, cyclic, reflective, none};


    /***********************************************************************/

    /**
     * Normal Constructor, which creates a grid with X*Y*Z elements
     * @param Xv length of X-Axis of the array == the length of the domain
     * @param Yv length of Y-Axis of the array == the length of the domain
     * @param Zv length of Z-Axis of the array == the length of the domain
     * @param rCutV the r_cut value
     * @param border the border types of the 6 (3D) or 4 (2D) borders
     */
    LinkedCellContainer(int Xv, int Yv, int Zv, double rCutV, std::array<Border, 6> borderV = std::array<Border, 6>{
            outflow, outflow, outflow, outflow, outflow, outflow
    }) :
            grid{std::vector<Cell>(static_cast<int>(std::floor(Xv/rCutV))*
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

    /**
     * @brief creates the cell grid
     */
    void setup() override;

    /**
     * @brief performs cleanup, deletes invalid Particles
     */
    void cleanup() override;

    void setLenDim(const std::array<int, 3> &lenDim);

    [[nodiscard]] size_t size() const noexcept override;

    void reserve(size_t) override;

    void emplace_back(Particle&& part) override;
    void emplace_back(Particle& part) override;
    void emplace_back(const std::array<double, 3>& x, const std::array<double, 3>& v, double m, int t) override;


    void push_back(const Particle&& p) override;
    void push_back(const Particle& p) override;

    std::vector<Particle>::iterator begin() override;

    std::vector<Particle>::iterator end() override;

    [[nodiscard]] std::vector<Particle>::const_iterator begin() const override;

    [[nodiscard]] std::vector<Particle>::const_iterator end() const override;

    /**
     * @brief Provides the iterator for single particles at the start of the collection
     * @return iterator
     */
    std::vector<Cell>::iterator begin_cell();

    /**
     * @brief Provides the iterator for single particles at the end of the collection
     * @return iterator
     */
    std::vector<Cell>::iterator end_cell();

    /**
     * @brief Provides the _const_ iterator for single Cells at the start of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Cell>::const_iterator begin_cell() const;

    /**
     * @brief Provides the _const_ iterator for single Cells at the end of the collection
     * @return const iterator
     */
    [[nodiscard]] std::vector<Cell>::const_iterator end_cell() const;

    /**
     * Calculates the distance between a position X a border
     * @param X the position
     * @param bord the specific border ([0;5])
     * @return the distance
     */
    [[nodiscard]] inline double getDistance(const std::array<double, 3> & X, int bord) const;

    /**
     * True if 2D, false if 3D
     * @return whether the grid is 2D or 3D
     */
    bool is2D();


//    /**
//     * Get a list of all neighbors of a particle with index i
//     * @tparam i The index of the Cell
//     * @return A vector of Cells that neighbor the cell
//     */
//    template<int i>
//    [[nodiscard]] std::vector<Cell> getNeighbors() const;

    /***** Getters *****/

    [[nodiscard]] const std::vector<Cell> & getGrid() const;

    [[nodiscard]] double getRCut() const;

    [[nodiscard]] const std::array<int, 3> &getDim() const;

    [[nodiscard]] const std::array<int, 3> &getLenDim() const;

    /**
     * Gets the border and the index of the border
     * LEFT, RIGHT, UP, DOWN, FRONT, BACK
     * @param currentIndexes the index of the current cell
     * @param d the axis we are currently in
     * @return
     */
    std::tuple<Border, int> getBorder(const std::array<int, 3> & currentIndexes, int d);


    /***** Setters *****/
    void setGrid(const std::vector<Cell> &grid);

    void setDim(const std::array<int, 3> &dim);

    void setRCut(double rCutV);

    [[nodiscard]] std::vector<std::array<int, 3>> getNeighbors (const std::array<int, 3> & currentIndex) const;

    /**
     * Has X * Y * Z many elements
     */
    std::vector<Cell> grid;
    std::vector<Particle> particles;

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

    std::array<Border, 6> border{outflow};
};

double LinkedCellContainer::getDistance(const std::array<double, 3> & X, int bord) const {
    switch (bord) {
        // LEFT
        case 0: return X[0];
            // RIGHT
        case 1: return lenDim[0] - X[0];
            // UPPER
        case 2: return X[1];
            // LOWER
        case 3: return lenDim[1] - X[1];
            // FRONT
        case 4: return X[2];
            // BACK
        case 5: return lenDim[2] - X[2];
            // INVALID CASE
        default: return -1;
    }
}

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
