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

ParticleIterator LinkedCellContainer::begin() {
    return LinkedCellParticleIterator{grid, 0, 0};
}

ParticleIterator LinkedCellContainer::end() {
    return LinkedCellParticleIterator{grid, grid.size(), grid[grid.size()-1].getParticles().size()};
}

ParticleIterator LinkedCellContainer::begin() const {
    return LinkedCellParticleIterator{grid, 0, 0};
}

ParticleIterator LinkedCellContainer::end() const {
    return LinkedCellParticleIterator{grid, grid.size(), grid[grid.size()-1].getParticles().size()};
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



