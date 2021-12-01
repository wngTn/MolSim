#include "LinkedCellContainer.h"
#include "LinkedCellParticleIterator.h"

const std::vector<Cell> & LinkedCellContainer::getGrid() const {
    return grid;
}

double LinkedCellContainer::getRCut() const {
    return rCut;
}

void LinkedCellContainer::setGrid(const std::vector<Cell> &gridV) {
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
    } else {
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

LinkedCellContainer::Border LinkedCellContainer::getBorder(const std::array<int, 3> &currentIndexes, int d) {
    // y value is zero --> upper border
    if (currentIndexes[1] == 0 && d == 1) {
        return border[2];
    }
    // x value is zero --> left border
    if (currentIndexes[0] == 0 && d == 0) {
        return border[0];
    }
    // y value is max --> lower border
    if (currentIndexes[1] == dim[1] - 1 && d == 1) {
        return border[3];
    }
    // x value is max --> right border
    if (currentIndexes[0] == dim[0] - 1 && d == 0) {
        return border[1];
    }
    // z value is zero --> front
    if (currentIndexes[2] == 0 && d == 2) {
        return border[4];
    }
    // z value is max --> back
    if (currentIndexes[2] == dim[2] - 1 && d == 2) {
        return border[5];
    }
}


std::vector<Particle>::iterator LinkedCellContainer::begin() {
    return std::vector<Particle>::iterator{};
}

std::vector<Particle>::iterator LinkedCellContainer::end() {
    return std::vector<Particle>::iterator{};}

std::vector<Particle>::const_iterator LinkedCellContainer::begin() const {
    return std::vector<Particle>::const_iterator{};
}

std::vector<Particle>::const_iterator LinkedCellContainer::end() const {
    return std::vector<Particle>::const_iterator{};
}

std::vector<Cell>::iterator LinkedCellContainer::begin_cell() {
    return grid.begin();
}

std::vector<Cell>::iterator LinkedCellContainer::end_cell() {
    return grid.end();
}

std::vector<Cell>::const_iterator LinkedCellContainer::begin_cell() const {
    return grid.begin();
}

std::vector<Cell>::const_iterator LinkedCellContainer::end_cell() const {
    return grid.end();
}



