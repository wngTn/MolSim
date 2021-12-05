#include <iostream>
#include "LinkedCell.h"

namespace calculator {

    void LinkedCell::calcFWithinCell(Cell &cell) {
        for (auto it = cell.begin(); it != cell.end(); ++it) {
            for (auto it2 = it; it2 != cell.end(); ++it2) {
                if (*it != *it2) {
                    double sqrd_dist = 0;
                    for (int i = 0; i < 3; i++) {
                        sqrd_dist += LinkedCell::sqr((*it2)->getX()[i] - (*it)->getX()[i]);
                    }
                    LinkedCell::ljforce((*it), (*it2), sqrd_dist);
                }
            }
        }

    }

    void LinkedCell::calcVcell(Cell &cell) const {
// calculate new velocities
        for (auto &p: cell) { // loop over every particle
            // go through all three dimensions
            auto newV = p->getV() + delta_t * 0.5 / p->getM() * (p->getF() + p->getOldF());
            p->setV(newV);
        }
    }

    // Method only valid for LinkedCellContainer. Parameter type is ParticleContainer to override
    // the cast is not very nice, but necessary to use the PhysicsCalc interface
    // distinguishing between LinkedCell and other calculators would make this cast
    // unnecessary, but would lead to lots of ugly code in every place calculators are used
    void LinkedCell::calcV(ParticleContainer &container) const {
        auto& gridLC = static_cast<LinkedCellContainer&>(container);
        for(auto& c : gridLC.grid){
            calcVcell(c);
        }
    }

    void LinkedCell::calcXcell(Cell &cell) const {
        // calculate new positions
        for (auto &p: cell) { // loop over every particle
            // go through all three dimensions
            auto newX = p->getX() + delta_t * (p->getV() + delta_t * 0.5 / p->getM() * p->getF());
            p->setX(newX);
        }
    }

    // Method only valid for LinkedCellContainer. Parameter type is ParticleContainer to override
    // the cast is not very nice, but necessary to use the PhysicsCalc interface
    // distinguishing between LinkedCell and other calculators would make this cast
    // unnecessary, but would lead to lots of ugly code in every place calculators are used
    void LinkedCell::calcX(ParticleContainer &container) const {
        auto& gridLC = static_cast<LinkedCellContainer&>(container);
       for(auto& c : gridLC.grid){
           calcXcell(c);
       }
       // TODO adapt moveParticles to only handle outflow and cyclic boundaryConditions
       moveParticles(gridLC);
    }

    void LinkedCell::moveParticles(LinkedCellContainer &grid) {
        std::array<int, 3> currentIndexes{}, novelIndex{};
        bool deleted = false;
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

                    auto & curCell = grid.grid[index(currentIndexes, grid.getDim())];

                    for (auto it = curCell.begin(); it != curCell.end(); ++it) {

                        for (int d = 0; d < 3; ++d) {
                            novelIndex[d] = static_cast<int>(std::floor(
                                    (*it)->getX()[d] * grid.getDim()[d] / grid.getLenDim()[d]));
                        }

                        // Checks whether any particle has crossed the boundaries
                        for (int d = 0; d < 3; ++d) {
                            if (novelIndex[d] < 0) {
                                // outflow, removing the particle
                                if (grid.getBorder(currentIndexes, d) == LinkedCellContainer::outflow) {
                                    std::cout<<"Removing Particle"<<std::endl;
                                    curCell.erase(it--);
                                    deleted = true;
                                    break;
                                }
                                    // cyclic
                                else if (grid.getBorder(currentIndexes, d) == LinkedCellContainer::cyclic) {
                                    novelIndex[d] = grid.getDim()[d] - 1;
                                }
                            } else if (novelIndex[d] >= grid.getDim()[d]) {
                                // outflow, removing the particle
                                if (grid.getBorder(currentIndexes, d) == LinkedCellContainer::outflow) {
                                    std::cout<<"Removing Particle"<<std::endl;
                                    curCell.erase(it--);
                                    deleted = true;
                                    break;
                                }
                                    // cyclic
                                else if (grid.getBorder(currentIndexes, d) == LinkedCellContainer::cyclic) {
                                    novelIndex[d] = 0;
                                }
                            }
                        }

                        // Check whether any index has changed, skip if we already deleted something
                        if ((currentIndexes[0] != novelIndex[0] || currentIndexes[1] != novelIndex[1] ||
                             currentIndexes[2] != novelIndex[2]) && !deleted) {
                            std::cout << "Moved Particle with type: " << (*it)->getType() << " from: (" <<
                                      currentIndexes[0] << ", " << currentIndexes[1] << ", " << currentIndexes[2]
                                      << ") to: " << "(" <<
                                      novelIndex[0] << ", " << novelIndex[1] << ", " << novelIndex[2] << ")"
                                      << std::endl;
                            // TODO STD::MOVE CURRENTLY DELETE AND CREATING
                            grid.grid[index(novelIndex, grid.getDim())].add_particle(*(*it));
                            curCell.erase(it--);
                        }
                        deleted = false;
                    }
                }
            }
        }
    }

    void LinkedCell::calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors,
                                   Particle* p) {
        // Loops through every particle of the neighbor
        for (auto &p_other: grid.grid[LinkedCell::index(neighbors, grid.getDim())]) {
            double sqrd_dist = 0;
            for (int i = 0; i < 3; i++) {
                sqrd_dist += LinkedCell::sqr(p_other->getX()[i] - p->getX()[i]);
            }
            if (sqrd_dist <= LinkedCell::sqr(rCut)) {
                LinkedCell::ljforce(p, p_other, sqrd_dist);
            }
            // DEBUGGEN
            else {
                std::cout << "Distance between Type: " << p->getType() << " and: " << p_other->getType()
                          << " is too high: " <<
                          sqrt(sqrd_dist) << std::endl;
            }
        }
    }

    void LinkedCell::reflectiveBoundary(LinkedCellContainer &grid, const std::array<int, 3> &currentIndexes) {
        //TODO reflective Boundary (apply force to particles close to border, if reflective border is activated for it)
    }

    void LinkedCell::calcF(ParticleContainer &container) {
        auto& grid = static_cast<LinkedCellContainer&>(container);
        for (auto &cell: grid.grid) {
            for (auto &p: cell) {
                p->setF({0, 0, 0});
            }
        }
        // current index we are currently in all 3 axis
        std::array<int, 3> currentIndexes{};
        // iterate through X axis
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

                    // TODO CHECK IF CELL IS A BORDER CELL AND THEN CHECK IF REFLECTION THEN DO REFLECTION ELSE DO NORMAL CELL CALCULATION
                    // Calculates the forces within a cell
                    calcFWithinCell(grid.grid[index(currentIndexes, grid.getDim())]);
                    // get the Cell in the current index
                    for (auto& p: grid.grid[index(currentIndexes, grid.getDim())]) {
                        // get all the neighbors
                        for (const std::array<int, 3> &neighbors: grid.getNeighbors(currentIndexes)) {
                            // TODO CHECK DISTANCE FROM CURRENT PARTICLE TO NEIGHBOR
                            // Neighbor should be existing
                            if (neighbors[0] < grid.getDim()[0] && neighbors[1] < grid.getDim()[1] &&
                                neighbors[2] < grid.getDim()[2]) {
                                calcNeighbors(grid, neighbors, p);
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