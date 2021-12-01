#include "LinkedCell.h"

namespace calculator {

    void LinkedCell::calcVcell(LinkedCellContainer::Cell & cell) const {
        // calculate new velocities
        for (auto &p: cell) { // loop over every particle
            // go through all three dimensions
            auto newV = p.getV() + delta_t * 0.5 / p.getM() * (p.getF() + p.getOldF());
            p.setV(newV);
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

    void LinkedCell::calcXcell(LinkedCellContainer::Cell &cell) const {
        // calculate new positions
        for (auto &p: cell) { // loop over every particle
            // go through all three dimensions
            auto newX = p.getX() + delta_t * ( p.getV() + delta_t * 0.5 / p.getM() * p.getF());
            p.setX(newX);
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

                    for (auto it = curCell.begin(); it != curCell.end();) {

                        // TODO treat boundary condition if it->getX() is out of bounds (only if using outflow/cyclic)

                        for (int d = 0; d < 3; ++d){
                            novelIndex[d] = static_cast<int>(std::floor(it->getX()[d] * grid.getDim()[d] / grid.getLenDim()[d]));
                        }

                        if (currentIndexes[0] != novelIndex[0] || currentIndexes[1] != novelIndex[1] || currentIndexes[2] != novelIndex[2]) {
                            std::cout<<"Moved Particle with type: "<<(*it).getType()<<" from: ("<<
                                     currentIndexes[0]<<", "<<currentIndexes[1]<<", "<<currentIndexes[2]<<") to: " <<"("<<
                                     novelIndex[0]<<", "<<novelIndex[1]<<", "<<novelIndex[2]<<")"<<std::endl;
                            // TODO STD::MOVE CURRENTLY DELETE AND CREATING
                            grid.grid[index(novelIndex, grid.getDim())].emplace_back((*it));
                            it = curCell.erase(it);
                        }
                        else {
                            ++it;
                        }
                    }
                }
            }
        }
    }

    void LinkedCell::calcNeighbors(LinkedCellContainer &grid, const std::array<int, 3> & neighbors,
                                   Particle &p){
        // TODO CHECK DISTANCE FROM CURRENT PARTICLE TO NEIGHBOR
        // TODO CHECK HOW TO APPLY NEWTON
        // std::cout<<"Currently at neighbor: ("<<
        //         neighbors[0]<<", "<<neighbors[1]<<", "<<neighbors[2]<<")"<<std::endl;
        for (auto & p_other : grid.grid[LinkedCell::index(neighbors, grid.getDim())]) {
     //       grid.grid[LinkedCell::index(neighbors, grid.getDim())].toString();
            if (p.operator!=(p_other)) {
                double sqrd_dist = 0;
                for (int i = 0; i < 3; i++) {
                    sqrd_dist += LinkedCell::sqr(p_other.getX()[i] - p.getX()[i]);
                }
                if (sqrd_dist <= LinkedCell::sqr(rCut)) {
                    LinkedCell::ljforce(p, p_other, sqrd_dist);
                }
                // DEBUGGEN
                else {
                    std::cout<<"Distance between Type: "<<p.getType()<<" and: "<<p_other.getType()<<" is too high: "<<
                    sqrt(sqrd_dist)<<std::endl;
                }
            }
        }
    }

    void LinkedCell::skipOutOfBoundary(LinkedCellContainer &grid, const std::array<int, 3> & neighbors,
                                       Particle &p) {
        // TODO
        for (int d = 0; (grid.getDim()[2] == 1) ? d < 2 : d < 3; d++) {
            if (neighbors[d] < 0 || neighbors[d] >= grid.getDim()[d])
                return;
        }
        calcNeighbors(grid, neighbors, p);
    }

    void LinkedCell::calcF_LC(LinkedCellContainer &grid) {
        // current index we are currently in all 3 axis
        std::array<int, 3> currentIndexes{}, neighbors{};
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

                    // get the Cell in the current index
                    for (auto & p : grid.grid[index(currentIndexes, grid.getDim())]) {

                        p.setF({0, 0, 0});

                        // iterate through all the neighbors in the X axis
                        for (neighbors[0] = currentIndexes[0] - 1;
                             neighbors[0] <= currentIndexes[0] + 1;
                                ++neighbors[0]) {
                            // iterate through all the neighbors in the Y axis
                            for (neighbors[1] = currentIndexes[1] - 1;
                                 neighbors[1] <= currentIndexes[1] + 1;
                                    ++neighbors[1]) {
                                // iterate through all the neighbors in the Z axis
                                // TODO find better method
                                for (neighbors[2] = (grid.getDim()[2] == 1) ? 0 : currentIndexes[2] - 1;
                                     (grid.getDim()[2] == 1) ? neighbors[2] <= 0 : neighbors[2] <= currentIndexes[2] + 1;
                                        ++neighbors[2]) {

                                    /* Boundary conditions */
                                    // only if using reflecting
                                    // if(distanceToBorder < LIMIT && usingReflecting)
                                    //      boundary(grid, p, distanceToBorder, currentIndexes)
                                    // grid: for dimensions
                                    // p: obvious
                                    // distanceToBorder: obvious
                                    // currentIndexes: to find out which border -> mirroring direction
                                    skipOutOfBoundary(grid, neighbors, p);
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