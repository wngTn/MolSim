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

const std::vector<std::array<int, 3>> &Cell::getNeighbors2D() const {
	return neighbors2D;
}

void Cell::setNeighbors2D1(const std::vector<std::array<int, 3>> &neighbors_2_d) {
	neighbors2D = neighbors_2_d;
}

const std::vector<std::array<int, 3>> &Cell::getNeighbors3D() const {
	return neighbors3D;
}

void Cell::setNeighbors3D1(const std::vector<std::array<int, 3>> &neighbors_3_d) {
	neighbors3D = neighbors_3_d;
}

bool Cell::isBorderCell1() const {
	return isBorderCell;
}

void Cell::setIsBorderCell(bool is_border_cell) {
	isBorderCell = is_border_cell;
}

void Cell::setAllNeighbors2D() {
	Cell::allNeighbors2D = {
//		std::array<int, 3>{index[0], index[1] + 1, index[2]},
//		std::array<int, 3>{index[0] + 1, index[1], index[2]},
//		std::array<int, 3>{index[0] + 1, index[1] + 1, index[2]},
//		std::array<int, 3>{index[0] - 1, index[1] + 1, index[2]},

		std::array<int, 3>{index[0], index[1] - 1, index[2]},
		std::array<int, 3>{index[0] - 1, index[1], index[2]},
		std::array<int, 3>{index[0] - 1, index[1] - 1, index[2]},
		std::array<int, 3>{index[0] + 1, index[1] - 1, index[2]},
	};
}

void Cell::setAllNeighbors3D() {
	Cell::allNeighbors3D = {
//		std::array<int, 3>{index[0], index[1] + 1, index[2]},
//		std::array<int, 3>{index[0] + 1, index[1], index[2]},
//		std::array<int, 3>{index[0] + 1, index[1] + 1, index[2]},
//		std::array<int, 3>{index[0] - 1, index[1] + 1, index[2]},
//		std::array<int, 3>{index[0], index[1], index[2] + 1},
//		std::array<int, 3>{index[0], index[1] + 1, index[2] + 1},
//		std::array<int, 3>{index[0] + 1, index[1], index[2] + 1},
//		std::array<int, 3>{index[0] + 1, index[1] + 1, index[2] + 1},
//		std::array<int, 3>{index[0] - 1, index[1], index[2] + 1},
//		std::array<int, 3>{index[0], index[1] - 1, index[2] + 1},
//		std::array<int, 3>{index[0] - 1, index[1] - 1, index[2] + 1},
//		std::array<int, 3>{index[0] - 1, index[1] + 1, index[2] + 1},
//		std::array<int, 3>{index[0] + 1, index[1] - 1, index[2] + 1},


		std::array<int, 3>{index[0], index[1] - 1, index[2]},
		std::array<int, 3>{index[0] - 1, index[1], index[2]},
		std::array<int, 3>{index[0] - 1, index[1] - 1, index[2]},
		std::array<int, 3>{index[0] + 1, index[1] - 1, index[2]},
		std::array<int, 3>{index[0], index[1], index[2] - 1},
		std::array<int, 3>{index[0], index[1] - 1, index[2] - 1},
		std::array<int, 3>{index[0] - 1, index[1], index[2] + 1},
		std::array<int, 3>{index[0] - 1, index[1] - 1, index[2] - 1},
		std::array<int, 3>{index[0] + 1, index[1], index[2] - 1},
		std::array<int, 3>{index[0], index[1] + 1, index[2] - 1},
		std::array<int, 3>{index[0] + 1, index[1] + 1, index[2] - 1},
		std::array<int, 3>{index[0] + 1, index[1] - 1, index[2] - 1},
		std::array<int, 3>{index[0] - 1, index[1] + 1, index[2] - 1}
	};


}

const std::vector<std::array<int, 3>> &Cell::getAllNeighbors2D() const {
	return allNeighbors2D;
}

const std::vector<std::array<int, 3>> &Cell::getAllNeighbors3D() const {
	return allNeighbors3D;
}


