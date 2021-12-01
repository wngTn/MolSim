#include "LinkedCellContainer.h"

const std::vector<LinkedCellContainer::Cell> & LinkedCellContainer::getGrid() const {
    return grid;
}

double LinkedCellContainer::getRCut() const {
    return rCut;
}

void LinkedCellContainer::setGrid(const std::vector<LinkedCellContainer::Cell> &gridV) {
    LinkedCellContainer::grid = gridV;
}

void LinkedCellContainer::setRCut(double rCutV) {
    LinkedCellContainer::rCut = rCutV;
}

const std::array<int, 3> &LinkedCellContainer::getDim() const {
    return dim;
}

void LinkedCellContainer::setDim(const std::array<int, 3> &dimV) {
    LinkedCellContainer::dim = dimV;
}

const std::array<int, 3> &LinkedCellContainer::getLenDim() const {
    return lenDim;
}

void LinkedCellContainer::setLenDim(const std::array<int, 3> &lenDimV) {
    LinkedCellContainer::lenDim = lenDimV;
}

LinkedCellContainer::Border LinkedCellContainer::getBorder() const {
    return border;
}

void LinkedCellContainer::setBorder(LinkedCellContainer::Border borderV) {
    LinkedCellContainer::border = borderV;
}

std::vector<std::array<int, 3>> LinkedCellContainer::getNeighbors(const std::array<int, 3> &currentIndex) const {
    // We are in 2D
    if (LinkedCellContainer::getDim()[2] == 1) {
        std::vector<std::array<int, 3>> neighbors(3);
        neighbors = {
                std::array<int, 3>{currentIndex[0], currentIndex[1] + 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1], currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1] + 1, currentIndex[2]}
        };
        return neighbors;
    }
    else {
        std::vector<std::array<int, 3>> neighbors(7);
        neighbors = {
                std::array<int, 3>{currentIndex[0], currentIndex[1], currentIndex[2] + 1},
                std::array<int, 3>{currentIndex[0], currentIndex[1] + 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0], currentIndex[1] + 1, currentIndex[2] + 1},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1], currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1], currentIndex[2] + 1},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1] + 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1] + 1, currentIndex[2] + 1}
        };
        return neighbors;
    }
}



std::vector<Particle>::iterator LinkedCellContainer::Cell::begin() {
    return particles.begin();
}

std::vector<Particle>::iterator LinkedCellContainer::Cell::end() {
    return particles.end();
}

std::vector<Particle>::const_iterator LinkedCellContainer::Cell::begin() const {
    return particles.begin();
}

std::vector<Particle>::const_iterator LinkedCellContainer::Cell::end() const {
    return particles.end();
}

std::vector<Particle>::iterator LinkedCellContainer::Cell::erase(std::vector<Particle>::const_iterator position) {
    return particles.erase(position);
}

void LinkedCellContainer::Cell::toString() {
    std::cout<<"Cell with: {";
    for (const auto & p : particles) {
        std::cout<<"Particle-"<<p.getType()<<", ";
    }
    std::cout<<"}"<<std::endl;
}


