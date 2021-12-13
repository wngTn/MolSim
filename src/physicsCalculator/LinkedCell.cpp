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
                                if (std::get<0>(grid.getBorder(currentIndexes, d)) == LinkedCellContainer::outflow) {
                                    spdlog::info("Removing Particle");
                                    it->valid = false;
                                    break;
                                }
                                    // cyclic
                                else if (std::get<0>(grid.getBorder(currentIndexes, d)) ==
                                         LinkedCellContainer::periodic) {
                                    // set X to the opposite site
                                    spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
                                                 it->getX()[0], it->getX()[1], it->getX()[2],
                                                 grid.getLenDim()[d] + it->getX()[d]);
                                    it->setX(d, grid.getLenDim()[d] + it->getX()[d]);
                                }
                            } else if (it->getX()[d] >= grid.getLenDim()[d]) {
                                // outflow, removing the particle
                                if (std::get<0>(grid.getBorder(currentIndexes, d)) == LinkedCellContainer::outflow) {
                                    spdlog::info("Removing Particle");
                                    it->valid = false;
                                    break;
                                }
                                    // cyclic
                                else if (std::get<0>(grid.getBorder(currentIndexes, d)) ==
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
        // saves the periodic borders of the current cell
        std::vector<int> perBorder{};
        // go through all three or two axis and acquire the borders of currentIndex that are reflective
        for (int d = 0; d < (grid.is2D() ? 2 : 3); ++d) {
            auto[bordType, bord] = grid.getBorder(currentIndexes, d);
            if (bordType == LinkedCellContainer::reflective) {
                reflBorder.push_back(bord);
            }
        }


        for (int d = 0; d < (grid.is2D() ? 2 : 3); ++d) {
            auto[bordType, bord] = grid.getBorder(currentIndexes, d);
            if (bordType == LinkedCellContainer::periodic) {
                perBorder.push_back(bord);
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
        for (auto &p: grid.grid[grid.index(currentIndexes)]) {
            if (!p->valid) {
                continue;
            }
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
                            spdlog::critical("DEFAULT CASE REFLECTIVE BORDER CALC SOMETHING WRONG ALARM");
                    }
                    p->setF(newF);
                }
            }

            for (int bord: perBorder) {
                // Indices of the mirrored neighbors
                std::vector<std::array<int, 3>> neigh{};
                // the mirror we are adding so that the particle gets mirrored
                std::array<double, 3> mirror{};
                switch (bord) {
                    // LEFT
                    case 0:
                        neigh = grid.getPerNeighbors(currentIndexes, 0);
                        mirror = {static_cast<double>(-grid.getLenDim()[0]), 0, 0};
                        break;
                    // RIGHT
                    case 1:
                        neigh = grid.getPerNeighbors(currentIndexes, 1);
                        mirror = {static_cast<double>(grid.getLenDim()[0]), 0, 0};
                        break;
                    // UP
                    case 2:
                        neigh = grid.getPerNeighbors(currentIndexes, 2);
                        mirror = {0, static_cast<double>(-grid.getLenDim()[1]), 0};
                        break;
                    // DOWN
                    case 3:
                        neigh = grid.getPerNeighbors(currentIndexes, 3);
                        mirror = {0, static_cast<double>(grid.getLenDim()[1]), 0};
                        break;
                    // FRONT
                    case 4:
                        neigh = grid.getPerNeighbors(currentIndexes, 4);
                        mirror = {0, 0, static_cast<double>(-grid.getLenDim()[2])};
                        break;
                    // BACK
                    case 5:
                        neigh = grid.getPerNeighbors(currentIndexes, 5);
                        mirror = {0, 0, static_cast<double>(grid.getLenDim()[2])};
                        break;
                    default:
                        spdlog::critical("DEFAULT CASE PERIODIC BORDER CALC SOMETHING WRONG ALARM");
                }
                for (const auto & n : neigh) {
                    LinkedCell::calcPerNeighbors(grid, n, p, mirror);
                }
            }
        }
    }

    void LinkedCell::calcPerNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors, Particle *p,
                                      const std::array<double, 3> & mirror) const {
        // Loop through the neighbors
        for (auto &p_other: grid.grid[grid.index(neighbors)]) {
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

                // set old force
                // p1->setOldF(p1->getF());
                // p2->setOldF(p2->getF());

                p->setF(p->getF() + force);
                p_other->setF(p_other->getF() - force);
            }
        }
    }

    void LinkedCell::calcF(ParticleContainer &container) {
        auto &grid = static_cast<LinkedCellContainer &>(container);
        for (auto &p: grid) {
            p.setOldF(p.getF());
            p.setF({0., 0., 0.});
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

}