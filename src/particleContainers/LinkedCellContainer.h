#pragma once

#include <vector>
#include <cmath>
#include <functional>
#include <spdlog/spdlog.h>


#include "Particle.h"
#include "ParticleContainer.h"
#include "Cell.h"

class LinkedCellContainer : public ParticleContainer{
public:

    enum Border {outflow, periodic, reflective, none};

    /***********************************************************************/

    /**
     * Normal Constructor, which creates a grid with X*Y*Z elements
     * @param Xv length of X-Axis of the array == the length of the domain
     * @param Yv length of Y-Axis of the array == the length of the domain
     * @param Zv length of Z-Axis of the array == the length of the domain
     * @param rCutV the r_cut value
     * @param borderV the border types of the 6 (3D) or 4 (2D) borders
     */
    LinkedCellContainer(int Xv, int Yv, int Zv, double rCutV, std::array<Border, 6> borderV = std::array<Border, 6>{
            outflow, outflow, outflow, outflow, outflow, outflow});

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

    PairIterator pair_begin() override;

    PairIterator pair_end() override;

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
     * Calculates whether the neighbor is in the range by calculating the longest distance of the middle point
     * of the neighbor cell to its edges and then calculating the distance between the middle point and the particle
     * and decide whether it should be considered.
     * If the distance is higher than the middle point distance + rCut, then it will not be considered
     * @param p the current particle
     * @param neighbor the neighbor cell index
     * @return true if it is in range, false if not
     * \note{Not used, since it did not affect our calculations much}
     */
    bool inline isNeighborInRange(const Particle * p, const std::array<int, 3> & neighbor);

    /**
     * Gets the border and the index of the border
     * 0: LEFT, 1: RIGHT, 2: UP, 3: DOWN, 4: FRONT, 5: BACK
     * @param currentIndexes the index of the current cell
     * @param d the axis we are currently in
     * @return
     */
    std::tuple<Border, int> getBorders(const std::array<int, 3> & currentIndexes, int d);

    /**
     * Returns a vector of neighbor indices depending on the dimension
     * @param currentIndex the current index of our particle
     * @return a vector of the indices of the neighbor
     */
    [[nodiscard]] std::vector<std::array<int, 3>> getNeighbors (const std::array<int, 3> & currentIndex) const;


    /**
     * Returns a vector of neighbor indices depending on the border it should be mirrored at
     * @param currentIndex the index of the current cell
     * @param borders the borders the current index is at
     * @return a vector of indices of the neighbor
     */
    std::vector<std::array<int, 3>> getPerNeighbors(const std::array<int, 3> & currentIndex);

    /**
     * Calculates the distance between a position X a border
     * @param X the position
     * @param bord the specific border ([0;5])
     * @return the distance
     */
    [[nodiscard]] inline double getDistance(const std::array<double, 3> & X, int bord) const;


    /**
     * Returns the index in the 1D Grid container
     * @param currentIndexes the current indexes in the three dimensions
     * @return The index
     */
    inline int index(const std::array<int , 3> & currentIndexes);


    /***** Getters *****/

    [[nodiscard]] const std::vector<Cell> & getGrid() const;

    [[nodiscard]] double getRCut() const;

    [[nodiscard]] const std::array<int, 3> &getDim() const;

    [[nodiscard]] const std::array<int, 3> &getLenDim() const;

    bool is2D();


    /***** Setters *****/
    void setGrid(const std::vector<Cell> &grid);

    void setDim(const std::array<int, 3> &dim);

    void setRCut(double rCutV);

    void setLenDim(const std::array<int, 3> &lenDim);

    const std::vector<Particle> &getParticles() const;

    void setParticles(const std::vector<Particle> &particles);

    const std::array<Border, 6> &getBorder() const;

    void setBorder(const std::array<Border, 6> &border);

    /**
     * Has X * Y * Z many elements
     * Vector where we save all our cells, which have the pointers to our particles
     */
    std::vector<Cell> grid;
    /**
     * Vector that saves all the instantiations of our particles
     */
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

    /**
     * This array describes the borders of our domain
     */
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

bool inline LinkedCellContainer::isNeighborInRange(const Particle *p, const std::array<int, 3> &neighbor) {
    // the max range from middle point of neighbor to its edges
    double midPointEdgeRange{};
    for (int d = 0; (dim[2] == 1) ? d < 2 : d < 3; ++d) {
        midPointEdgeRange += static_cast<double>(lenDim[d]) / static_cast<double>(dim[d]) *
                static_cast<double>(lenDim[d]) / static_cast<double>(dim[d]);
    }
    midPointEdgeRange = sqrt(midPointEdgeRange);

    // position of the neighborCell middle point
    std::array<double, 3> neighborPos{};
    for (int d = 0; (dim[2] == 1) ? d < 2 : d < 3; ++d) {
        neighborPos[d] = neighbor[d] * (static_cast<double>(lenDim[d]) / static_cast<double>(dim[d]));
    }

    // Distance between these points
    double distance{};
    for (int d = 0; (dim[2] == 1) ? d < 2 : d < 3; ++d) {
        distance += (neighborPos[d] - p->getX()[d]) * (neighborPos[d] - p->getX()[d]);
    }
    distance = sqrt(distance);

    return distance <= midPointEdgeRange + rCut;
}

int LinkedCellContainer::index(const std::array<int, 3> &currentIndexes) {
    return currentIndexes[0] + dim[0] * (currentIndexes[1] + dim[1] * currentIndexes[2]);
}