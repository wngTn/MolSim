#include "LinkedCell.h"

namespace calculator {

    void LinkedCell::calcF(LinkedCellContainer::Cell &cell) {
        if (cell.getParticles().size() > 1) {
//            for (auto &p: cell) { // loop over every particle
//                p.setF({0., 0., 0.});
//            }

            for (auto it = cell.begin(); it != cell.end(); ++it) {
                for (auto it2 = it; it2 != cell.end(); ++it2) {
                    if (*it != *it2) {
                        double sqrd_dist = 0;
                        for (int i = 0; i < 3; i++) {
                            sqrd_dist += LinkedCell::sqr((*it2).getX()[i] - (*it).getX()[i]);
                        }
                        LinkedCell::ljforce((*it), (*it2), sqrd_dist);
                    }
                }
            }
        }
    }

    void LinkedCell::calcV(LinkedCellContainer::Cell &cell) const {
// calculate new velocities
        for (auto &p: cell) { // loop over every particle
            // go through all three dimensions
            auto newV = p.getV() + delta_t * 0.5 / p.getM() * (p.getF() + p.getOldF());
            p.setV(newV);
        }
    }

    void LinkedCell::compV_LC(LinkedCellContainer &grid) const {
        std::array<int, 3> currentIndexes{};
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {
                    calcV(grid.grid[index(currentIndexes, grid.getDim())]);
                }
            }
        }
    }

    void LinkedCell::calcX(LinkedCellContainer::Cell &cell) const {
// calculate new positions
        for (auto &p: cell) { // loop over every particle
            // go through all three dimensions
            auto newX = p.getX() + delta_t * (p.getV() + delta_t * 0.5 / p.getM() * p.getF());
            p.setX(newX);
        }
    }

    void LinkedCell::compX_LC(LinkedCellContainer &grid) const {
        std::array<int, 3> currentIndexes{};
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {
                    calcX(grid.grid[index(currentIndexes, grid.getDim())]);
                }
            }
        }
        moveParticles(grid);
    }

    void LinkedCell::moveParticles(LinkedCellContainer &grid) {
        std::array<int, 3> currentIndexes{}, novelIndex{};
        bool deleted = false;
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

                    auto &curCell = grid.grid[index(currentIndexes, grid.getDim())];

                    for (auto it = curCell.begin(); it != curCell.end();) {

                        for (int d = 0; d < 3; ++d) {
                            novelIndex[d] = static_cast<int>(std::floor(
                                    it->getX()[d] * grid.getDim()[d] / grid.getLenDim()[d]));
                        }

                        // Checks whether any particle has crossed the boundaries
                        for (int d = 0; d < 3; ++d) {
                            if (novelIndex[d] < 0) {
                                // outflow, removing the particle
                                if (grid.getBorder() == LinkedCellContainer::outflow) {
                                    it = curCell.erase(it);
                                    deleted = true;
                                }
                                    // cyclic
                                else if (grid.getBorder() == LinkedCellContainer::cyclic) {
                                    novelIndex[d] = grid.getDim()[d] - 1;
                                }
                            } else if (novelIndex[d] >= grid.getDim()[d]) {
                                // outflow, removing the particle
                                if (grid.getBorder() == LinkedCellContainer::outflow) {
                                    it = curCell.erase(it);
                                    deleted = true;
                                }
                                    // cyclic
                                else if (grid.getBorder() == LinkedCellContainer::cyclic) {
                                    novelIndex[d] = 0;
                                }
                            }
                        }

                        if ((currentIndexes[0] != novelIndex[0] || currentIndexes[1] != novelIndex[1] ||
                             currentIndexes[2] != novelIndex[2]) && !deleted) {
                            std::cout << "Moved Particle with type: " << (*it).getType() << " from: (" <<
                                      currentIndexes[0] << ", " << currentIndexes[1] << ", " << currentIndexes[2]
                                      << ") to: " << "(" <<
                                      novelIndex[0] << ", " << novelIndex[1] << ", " << novelIndex[2] << ")"
                                      << std::endl;
                            // TODO STD::MOVE CURRENTLY DELETE AND CREATING
                            grid.grid[index(novelIndex, grid.getDim())].emplace_back((*it));
                            it = curCell.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
            }
        }
    }

    void LinkedCell::calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors,
                                   Particle &p) {
// TODO CHECK DISTANCE FROM CURRENT PARTICLE TO NEIGHBOR
// std::cout<<"Currently at neighbor: ("<<
//         neighbors[0]<<", "<<neighbors[1]<<", "<<neighbors[2]<<")"<<std::endl;
        for (auto &p_other: grid.grid[LinkedCell::index(neighbors, grid.getDim())]) {
//       grid.grid[LinkedCell::index(neighbors, grid.getDim())].toString();
                double sqrd_dist = 0;
                for (int i = 0; i < 3; i++) {
                    sqrd_dist += LinkedCell::sqr(p_other.getX()[i] - p.getX()[i]);
                }
                if (sqrd_dist <= LinkedCell::sqr(rCut)) {
                    LinkedCell::ljforce(p, p_other, sqrd_dist);
                }
                    // DEBUGGEN
                else {
                    std::cout << "Distance between Type: " << p.getType() << " and: " << p_other.getType()
                              << " is too high: " <<
                              sqrt(sqrd_dist) << std::endl;
                }
        }
    }

    void LinkedCell::reflectiveBoundary(LinkedCellContainer &grid, const std::array<int, 3> &neighbors,
                                        Particle &p) {
//TODO
    }

    void LinkedCell::calcF_LC(LinkedCellContainer &grid, bool reflection) {
        for (auto & cell : grid.grid) {
            for (auto & p : cell) {
                p.setF({0, 0, 0});
            }
        }
// current index we are currently in all 3 axis
        std::array<int, 3> currentIndexes{};
// the neighbors in all three dimensions
// iterate through X axis
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

//                    if (!grid.grid[index(currentIndexes, grid.getDim())].getParticles().empty()) {
//                        std::cout<<"Currently at cell: ("<<currentIndexes[0]<<", "<<currentIndexes[1]<<", "<<currentIndexes[2]<<")"<<std::endl;
//                        std::cout << "This cell has: " << grid.grid[index(currentIndexes, grid.getDim())].getParticles().size() << " particles" << std::endl;
//                    }
                    calcF(grid.grid[index(currentIndexes, grid.getDim())]);
                    // get the Cell in the current index
                    for (auto &p: grid.grid[index(currentIndexes, grid.getDim())]) {
                        // get all the neighbors
                        for (const std::array<int, 3> &neighbors: grid.getNeighbors(currentIndexes)) {
                            // Neighbor should be existing
                            if (neighbors[0] < grid.getDim()[0] && neighbors[1] < grid.getDim()[1] &&
                                neighbors[2] < grid.getDim()[2]) {
                                if (reflection) {
                                    reflectiveBoundary(grid, neighbors, p);
                                } else {
                                    calcNeighbors(grid, neighbors, p);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::string LinkedCell::toString() {
        return "LinkedCell";
    }

}