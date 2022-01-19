#include <iostream>
#include "Cell.h"


std::vector<Particle*>::iterator Cell::begin() {
    return particles.begin();
}

std::vector<Particle*>::iterator Cell::end() {
    return particles.end();
}

std::vector<Particle*>::const_iterator Cell::begin() const {
    return particles.begin();
}

std::vector<Particle*>::const_iterator Cell::end() const {
    return particles.end();
}

std::vector<Particle*>::iterator Cell::erase(std::vector<Particle*>::const_iterator position) {
    return particles.erase(position);
}

void Cell::add_particle(Particle& p){
    Particle* pp = &p;
    particles.emplace_back(pp);
}

void Cell::toString() {
    std::cout<<"Cell with: {";
    for (const auto & p : particles) {
        std::cout<<"Particle-"<<p->getType()<<", ";
    }
    std::cout<<"}"<<std::endl;
}

void Cell::clear() {
    particles.clear();
}

const std::array<int, 3> &Cell::getIndex() const {
    return index;
}

void Cell::setIndex(const std::array<int, 3> &indexV) {
    Cell::index = indexV;
}

const std::vector<std::array<int, 3>> &Cell::getNeighbors2D() const {
    return neighbors2D;
}

void Cell::setNeighbors2D(const std::vector<std::array<int, 3>> &neighbors2DV) {
    Cell::neighbors2D = neighbors2DV;
}

const std::vector<std::array<int, 3>> &Cell::getNeighbors3D() const {
    return neighbors3D;
}

void Cell::setNeighbors3D(const std::vector<std::array<int, 3>> &neighbors3DV) {
    Cell::neighbors3D = neighbors3DV;
}

Cell::Cell() : particles{std::vector<Particle*>{}}{}

void Cell::setNeighbors2D() {
    Cell::neighbors2D = {
            std::array<int, 3>{index[0], index[1] + 1, index[2]},
            std::array<int, 3>{index[0] + 1, index[1], index[2]},
            std::array<int, 3>{index[0] + 1, index[1] + 1, index[2]},
            std::array<int, 3>{index[0] - 1, index[1] + 1, index[2]}
    };
}

void Cell::setNeighbors3D() {
    Cell::neighbors3D = {
            std::array<int, 3>{index[0], index[1] + 1, index[2]},
            std::array<int, 3>{index[0] + 1, index[1], index[2]},
            std::array<int, 3>{index[0] + 1, index[1] + 1, index[2]},
            std::array<int, 3>{index[0] - 1, index[1] + 1, index[2]},
            std::array<int, 3>{index[0], index[1], index[2] + 1},
            std::array<int, 3>{index[0], index[1] + 1, index[2] + 1},
            std::array<int, 3>{index[0] + 1, index[1], index[2] + 1},
            std::array<int, 3>{index[0] + 1, index[1] + 1, index[2] + 1},
            std::array<int, 3>{index[0] - 1, index[1], index[2] + 1},
            std::array<int, 3>{index[0], index[1] - 1, index[2] + 1},
            std::array<int, 3>{index[0] - 1, index[1] - 1, index[2] + 1},
            std::array<int, 3>{index[0] - 1, index[1] + 1, index[2] + 1},
            std::array<int, 3>{index[0] + 1, index[1] - 1, index[2] + 1}
    };
}

bool Cell::isBorderCell1() const {
    return isBorderCell;
}

void Cell::setIsBorderCell(bool ibc) {
    Cell::isBorderCell = ibc;
}



