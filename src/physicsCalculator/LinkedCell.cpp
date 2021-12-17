#include <iostream>
#include "LinkedCell.h"

namespace calculator {

    void LinkedCell::calcFWithinCell(Cell &cell) {
        for (auto it = cell.begin(); it != cell.end(); ++it) {
            for (auto it2 = it; it2 != cell.end(); ++it2) {
                if (*it != *it2) {
                    double sqrd_dist = 0;
                    for (int i = 0; i < DIM; i++) {
                        sqrd_dist += LinkedCell::sqr((*it2)->getX()[i] - (*it)->getX()[i]);
                    }
                    LinkedCell::ljforce((*it), (*it2), sqrd_dist);
                }
            }
        }

    }

    void LinkedCell::calcX(ParticleContainer &container) const {
        auto &gridLC = static_cast<LinkedCellContainer &>(container);
        for (auto &p: gridLC) {
            auto newX = p.getX() + delta_t * (p.getV() + delta_t * 0.5 / p.getM() * p.getF());
            p.setX(newX);
        }
        moveParticles(gridLC);
    }

    void LinkedCell::moveParticles(LinkedCellContainer &grid) {
        std::array<int, 3> currentIndexes{};
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

                    auto &curCell = grid.grid[grid.index(currentIndexes)];

                    for (auto &it: curCell) {
                        /*if(!it->valid){
                            continue;
                        }*/
                        // Checks whether any particle has crossed the boundaries
                        for (int d = 0; d < 3; ++d) {
                            if (it->getX()[d] < 0) {
                                // outflow, removing the particle
                                if (std::get<0>(grid.getBorders(currentIndexes, d)) == LinkedCellContainer::outflow) {
                                    spdlog::info("Removing Particle");
                                    it->valid = false;
                                    break;
                                }
                                    // periodic
                                else if (std::get<0>(grid.getBorders(currentIndexes, d)) ==
                                         LinkedCellContainer::periodic) {
                                    // set X to the opposite site
                                    spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
                                                 it->getX()[0], it->getX()[1], it->getX()[2],
                                                 grid.getLenDim()[d] + it->getX()[d]);
                                    it->setX(d, grid.getLenDim()[d] + it->getX()[d]);
                                }
                            } else if (it->getX()[d] >= grid.getLenDim()[d]) {
                                // outflow, removing the particle
                                if (std::get<0>(grid.getBorders(currentIndexes, d)) == LinkedCellContainer::outflow) {
                                    spdlog::info("Removing Particle");
                                    it->valid = false;
                                    break;
                                }
                                    // periodic
                                else if (std::get<0>(grid.getBorders(currentIndexes, d)) ==
                                         LinkedCellContainer::periodic) {
                                    // set X to the opposite site
                                    spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
                                                 it->getX()[0], it->getX()[1], it->getX()[2],
                                                 it->getX()[d] - grid.getLenDim()[d]);
                                    it->setX(d, it->getX()[d] - grid.getLenDim()[d]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void LinkedCell::calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors,
                                   Particle *p) {
        // Loops through every particle of the neighbor
        for (auto &p_other: grid.grid[grid.index(neighbors)]) {
            double sqrd_dist = 0;
            for (int i = 0; i < DIM; i++) {
                sqrd_dist += LinkedCell::sqr(p_other->getX()[i] - p->getX()[i]);
            }
            if (sqrd_dist <= LinkedCell::sqr(rCut)) {
                LinkedCell::ljforce(p, p_other, sqrd_dist);
            }
        }
    }

    void LinkedCell::reflectiveBoundary(LinkedCellContainer &grid, const std::array<int, 3> &currentIndexes) const {
        // We only reflect at this distance
        double reflectDistance = std::pow(2, 1.0 / 6.0) * sigma;
        // saves the reflective borders of the current cell
        std::vector<int> reflBorder{};
        // go through all three or two axis and acquire the borders of currentIndex that are reflective
        for (int d = 0; d < (grid.is2D() ? 2 : 3); ++d) {
            auto[bordType, bord] = grid.getBorders(currentIndexes, d);
            if (bordType == LinkedCellContainer::reflective) {
                reflBorder.push_back(bord);
            }
        }

        /**
         * Border type overview:
         * 0: LEFT
         * 1: RIGHT
         * 2: UPPER
         * 3: LOWER
         * 4: FRONT
         * 5: BACK
         */
        if (!reflBorder.empty()) {
            for (auto &p: grid.grid[grid.index(currentIndexes)]) {
//                if (!p->valid) {
//                    continue;
//                }
                for (int bord: reflBorder) {
                    double r = grid.getDistance(p->getX(), bord);
                    if (r <= reflectDistance) {
                        double s = (sigma * sigma) / (r * r);
                        s = s * s * s;
                        auto force = -24 * epsilon / r * s * (1 - 2 * s);
                        // auto force = -24 * epsilon * (1/(r)) * pow((sigma/(r)), 6) * (1 - 2 * (pow((sigma/(r)), 6)));
                        auto newF{p->getF()};
                        switch (bord) {
                            case 0:
                                newF[0] += force;
                                break;
                            case 1:
                                newF[0] -= force;
                                break;
                            case 2:
                                newF[1] += force;
                                break;
                            case 3:
                                newF[1] -= force;
                                break;
                            case 4:
                                newF[2] += force;
                                break;
                            case 5:
                                newF[2] -= force;
                                break;
                            default:
                                spdlog::critical("DEFAULT CASE SOMETHING WRONG ALARM");
                        }
                        p->setF(newF);
                    }
                }
            }
        }
    }

    void LinkedCell::calcPerNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors, Particle *p,
                                      const std::array<double, 3> & mirror) const {
        // Loop through the neighbors
        for (auto &p_other: grid.grid[grid.index(neighbors)]) {
            // This if is important if the domain only has one cell
            if (p != p_other) {
                auto mirroredX = p_other->getX() + mirror;
                double sqrd_dist = 0;
                for (int i = 0; i < DIM; i++) {
                    sqrd_dist += LinkedCell::sqr(mirroredX[i] - p->getX()[i]);
                }
                if (sqrd_dist <= LinkedCell::sqr(rCut)) {
                    double s = sqr(sigma) / sqrd_dist;
                    s = s * s * s; // s = sqr(s) * s
                    double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);

                    auto force = f * (mirroredX - p->getX());

                    p->setF(p->getF() + force);
                    p_other->setF(p_other->getF() - force);
                }
            }
        }
    }

    void LinkedCell::calcF(ParticleContainer &container) {
        auto &grid = static_cast<LinkedCellContainer &>(container);
        for (auto &p: grid) {
            p.setOldF(p.getF());
            p.setF({0., p.getM() * g, 0.});
        }
        // current index we are currently in all 3 axis
        std::array<int, 3> currentIndexes{};
        // iterate through X axis
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

                    // get the Cell in the current index
                    for (auto &p: grid.grid[grid.index(currentIndexes)]) {
                        // get all the neighbors
                        for (const std::array<int, 3> &neighbors: grid.getNeighbors(currentIndexes)) {

                            // Neighbor should be existing
                            if (neighbors[0] < grid.getDim()[0] && neighbors[1] < grid.getDim()[1] &&
                                neighbors[2] < grid.getDim()[2] && neighbors[0] >= 0 && neighbors[1] >= 0 &&
                                neighbors[2] >= 0) {
                                // Does not bring too much performance benefits
                                //if (grid.isNeighborInRange(p, neighbors)) {
                                calcNeighbors(grid, neighbors, p);
                                //}
                            }
                            // Else it is a neighbor out of boundary
                            else if (grid.isPeriodic(neighbors)){
                                // neighbor on the other side
                                std::array<int, 3> neigh{};
                                for (int d = 0; d < 3; ++d) {
                                    neigh[d] = (neighbors[d] + grid.getDim()[d]) % grid.getDim()[d];
                                }
                                // the mirror we are adding so that the particle gets mirrored
                                std::array<double, 3> mirror{};
                                mirror = {
                                        neighbors[0] == -1 ? static_cast<double>(-grid.getLenDim()[0]) : // From left to right
                                        neighbors[0] == grid.getDim()[0] ? static_cast<double>(grid.getLenDim()[0]) : 0.0, // From right to left
                                        neighbors[1] == -1 ? static_cast<double>(-grid.getLenDim()[1]) :
                                        neighbors[1] == grid.getDim()[1] ? static_cast<double>(grid.getLenDim()[1]) : 0.0,
                                        neighbors[2] == -1 ? static_cast<double>(-grid.getLenDim()[2]) :
                                        neighbors[2] == grid.getDim()[2]  ? static_cast<double>(grid.getLenDim()[2]) : 0.0};
                                LinkedCell::calcPerNeighbors(grid, neigh, p, mirror);
                            }
                        }
                    }

                    // Calculates the forces within a cell
                    calcFWithinCell(grid.grid[grid.index(currentIndexes)]);
                    // checks if it is a border cell, if yes also calculate border forces
                    if (currentIndexes[0] == 0 || currentIndexes[0] == grid.getDim()[0] - 1 ||
                        currentIndexes[1] == 0 || currentIndexes[1] == grid.getDim()[1] - 1 ||
                        currentIndexes[2] == 0 || currentIndexes[2] == grid.getDim()[2] - 1) {
                        reflectiveBoundary(grid, currentIndexes);
                    }
                }
            }
        }
    }

    std::string LinkedCell::toString() {
        return "LinkedCell";
    }

    /**
     * Deprecated
     */
    Particle LinkedCell::generateGhostParticle(const LinkedCellContainer &grid, const Particle *p, int bord) {
        Particle ghostParticle{};
        switch (bord) {
            // LEFT
            case 0: {
                // Mirror at Y axis
                auto mirror = std::array<double, 3>{-1, 1, 1};
                auto newX = mirror * p->getX();
                auto newF = mirror * p->getF();
                ghostParticle.setX(newX);
                ghostParticle.setF(newF);
                return ghostParticle;
            }
                // RIGHT
            case 1: {
                // Mirror for position at right border
                auto mirrorX = std::array<double, 3>{2 * grid.getDistance(p->getX(),
                                                                          1), 0, 0};
                auto newX = mirrorX + p->getX();
                // Mirror at Y axis
                auto mirror = std::array<double, 3>{-1, 1, 1};
                auto newF = mirror * p->getF();
                ghostParticle.setX(newX);
                ghostParticle.setF(newF);
                return ghostParticle;
            }
                // UPPER
            case 2: {
                // Mirror at X axis
                auto mirror = std::array<double, 3>{1, -1, 1};
                auto newX = mirror * p->getX();
                auto newF = mirror * p->getF();
                ghostParticle.setX(newX);
                ghostParticle.setF(newF);
                return ghostParticle;
            }
                // LOWER
            case 3: {
                // Mirror for position at lower border
                auto mirrorX = std::array<double, 3>{0, 2 * grid.getDistance(p->getX(),
                                                                             3), 0};
                auto newX = mirrorX + p->getX();
                // Mirror at X axis
                auto mirror = std::array<double, 3>{1, -1, 1};
                auto newF = mirror * p->getF();
                ghostParticle.setX(newX);
                ghostParticle.setF(newF);
                return ghostParticle;
            }
                // FRONT
            case 4: {
                // Mirror at Z axis
                auto mirror = std::array<double, 3>{1, 1, -1};
                auto newX = mirror * p->getX();
                auto newF = mirror * p->getF();
                ghostParticle.setX(newX);
                ghostParticle.setF(newF);
                return ghostParticle;
            }
                // BACK
            case 5: {
                // Mirror for position at lower border
                auto mirrorX = std::array<double, 3>{0, 0, 2 * grid.getDistance(p->getX(),
                                                                                5)};
                auto newX = mirrorX + p->getX();
                // Mirror at X axis
                auto mirror = std::array<double, 3>{1, 1, -1};
                auto newF = mirror * p->getF();
                ghostParticle.setX(newX);
                ghostParticle.setF(newF);
                return ghostParticle;
            }
                // Shouldn't get to this
            default: {
                return ghostParticle;
            }
        }
    }

}