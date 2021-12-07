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
        for (currentIndexes[0] = 0; currentIndexes[0] < grid.getDim()[0]; ++currentIndexes[0]) {
            // iterate through the Y axis
            for (currentIndexes[1] = 0; currentIndexes[1] < grid.getDim()[1]; ++currentIndexes[1]) {
                // iterate through the Z axis
                for (currentIndexes[2] = 0; currentIndexes[2] < grid.getDim()[2]; ++currentIndexes[2]) {

                    auto & curCell = grid.grid[index(currentIndexes, grid.getDim())];

                    for (auto & it : curCell) {

                        for (int d = 0; d < 3; ++d) {
                            novelIndex[d] = static_cast<int>(std::floor(
                                    it->getX()[d] * grid.getDim()[d] / grid.getLenDim()[d]));
                        }

                        // Checks whether any particle has crossed the boundaries
                        for (int d = 0; d < 3; ++d) {
                            if (novelIndex[d] < 0) {
                                // outflow, removing the particle
                                if (std::get<0>(grid.getBorder(currentIndexes, d)) == LinkedCellContainer::outflow) {
                                    std::cout<<"Removing Particle"<<std::endl;
                                    it->valid = false;
                                    break;
                                }
                                    // cyclic
                                else if (std::get<0>(grid.getBorder(currentIndexes, d)) == LinkedCellContainer::cyclic) {
                                    // set X to the opposite site
                                    std::cout<<"Particle was at d: "<<d<<" and position"<<it->getX()<<"; now at "<<
                                    grid.getLenDim()[d] + it->getX()[d]<<std::endl;
                                    it->setX(d, grid.getLenDim()[d] + it->getX()[d]);
                                }
                            } else if (novelIndex[d] >= grid.getDim()[d]) {
                                // outflow, removing the particle
                                if (std::get<0>(grid.getBorder(currentIndexes, d)) == LinkedCellContainer::outflow) {
                                    std::cout<<"Removing Particle"<<std::endl;
                                    it->valid = false;
                                    break;
                                }
                                    // cyclic
                                else if (std::get<0>(grid.getBorder(currentIndexes, d)) == LinkedCellContainer::cyclic) {
                                    // set X to the opposite site
                                    std::cout<<"Particle was at d: "<<d<<" and position"<<it->getX()<<"; now at "<<
                                    it->getX()[d] - grid.getLenDim()[d]<<std::endl;
                                    it->setX(d, it->getX()[d] - grid.getLenDim()[d]);
                                }
                            }
                        }

//                        // Check whether any index has changed, skip if we already deleted something
//                        if ((currentIndexes[0] != novelIndex[0] || currentIndexes[1] != novelIndex[1] ||
//                             currentIndexes[2] != novelIndex[2]) && !deleted) {
//                            std::cout << "Moved Particle with type: " << (*it)->getType() << " from: (" <<
//                                      currentIndexes[0] << ", " << currentIndexes[1] << ", " << currentIndexes[2]
//                                      << ") to: " << "(" <<
//                                      novelIndex[0] << ", " << novelIndex[1] << ", " << novelIndex[2] << ")"
//                                      << std::endl;
//                            grid.grid[index(novelIndex, grid.getDim())].add_particle(*(*it));
//                            curCell.erase(it--);
//                        }
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
//            else {
//                std::cout << "Distance between Type: " << p->getType() << " and: " << p_other->getType()
//                          << " is too high: " <<
//                          sqrt(sqrd_dist) << std::endl;
//            }
        }
    }

    void LinkedCell::reflectiveBoundary(LinkedCellContainer &grid, const std::array<int, 3> &currentIndexes) {
        // We only reflect at this distance
        double reflectDistance = std::pow(2, 1.0/6.0) * sigma;
        // saves the borders of the current cell
        std::vector<int> borders;
        // go through all three or two axis and acquire the borders of currentIndex that are reflective
        for (int d = 0; d < (grid.is2D() ? 2 : 3); ++d) {
            auto [bordType, bord] = grid.getBorder(currentIndexes, d);
            if (bordType == LinkedCellContainer::reflective) {
                borders.push_back(bord);
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

        for (auto & p : grid.grid[index(currentIndexes, grid.getDim())]) {
            for (int bord : borders) {
                if (grid.getDistance(p->getX(), bord) <= reflectDistance) {
                    Particle p2 = generateGhostParticle(grid, p, bord);
                    double sqrd_dist = 0;
                    for (int i = 0; i < 3; i++) {
                        sqrd_dist += LinkedCell::sqr(p2.getX()[i] - p->getX()[i]);
                    }
                    std::cout<<"THIS IS GHOST PARTICLE CALCULATION"<<std::endl;
                    std::cout<<"Particle with Type: "<<p->getType()
                    <<" before Force Calc is: ("<<p->getF()[0]<<", "<<p->getF()[1]<<", "<<p->getF()[2]<<")"<<std::endl;
                    std::cout<<"At border: "<<bord<<std::endl;
                    LinkedCell::ljforce(p, &p2, sqrd_dist);
                }
            }
        }
    }

    Particle LinkedCell::generateGhostParticle(const LinkedCellContainer & grid, const Particle* p, int bord){
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
                    // checks if it is a border cell, if yes also calculate border forces
                    if(currentIndexes[0] == 0 || currentIndexes[0] == grid.getDim()[0] - 1 ||
                       currentIndexes[1] == 0 || currentIndexes[1] == grid.getDim()[1] - 1 ||
                       currentIndexes[2] == 0 || currentIndexes[2] == grid.getDim()[2] - 1) {
                        reflectiveBoundary(grid, currentIndexes);

                    }
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