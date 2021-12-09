
#include <iostream>
#include "LinkedCellContainer.h"



LinkedCellContainer::LinkedCellContainer(int Xv, int Yv, int Zv, double rCutV, std::array<Border, 6> borderV) :
        grid{std::vector<Cell>(static_cast<int>(std::floor(Xv/rCutV))*
                               static_cast<int>(std::floor(Yv/rCutV))*
                               (static_cast<int>(std::floor(Zv/rCutV)) == 0 ? 1 :
                                static_cast<int>(std::floor(Zv/rCutV))))},
        dim{std::array<int, 3>{static_cast<int>(std::floor(Xv/rCutV)),
                               static_cast<int>(std::floor(Yv/rCutV)),
                               (static_cast<int>(std::floor(Zv/rCutV))) == 0 ? 1 :
                               static_cast<int>(std::floor(Zv/rCutV))}},
        lenDim{std::array<int, 3>{Xv, Yv, Zv}}, rCut{rCutV}, border{borderV} {}


void LinkedCellContainer::setup() {
    for(auto & it : grid){
        it = Cell{};
    }
    for(auto &p : particles){
        if(p.valid){
            std::array<int, 3> novelCellIndex{};
            for (int d = 0; d < 3; ++d) {
                novelCellIndex[d] = static_cast<int>(std::floor(
                        p.getX()[d] * getDim()[d] / getLenDim()[d]));
            }
            auto cellIndex = (*this).index(novelCellIndex);
            grid[cellIndex].emplace_back(&p);
        }
    }
}

void LinkedCellContainer::cleanup() {
    // use erase-remove idiom
    particles.erase(std::remove_if(particles.begin(),
                                   particles.end(), [](Particle& p){return !p.valid;}), particles.end());
}

std::vector<std::array<int, 3>> LinkedCellContainer::getNeighbors(const std::array<int, 3> &currentIndex) const {
    // We are in 2D
    if (LinkedCellContainer::getDim()[2] == 1) {
        std::vector<std::array<int, 3>> neighbors(4);
        neighbors = {
                std::array<int, 3>{currentIndex[0], currentIndex[1] + 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1], currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1] + 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] - 1, currentIndex[1] + 1, currentIndex[2]}

                /*,
                std::array<int, 3>{currentIndex[0], currentIndex[1] - 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] - 1, currentIndex[1], currentIndex[2]},
                std::array<int, 3>{currentIndex[0] - 1, currentIndex[1] - 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] -1 , currentIndex[1] + 1, currentIndex[2]},
                std::array<int, 3>{currentIndex[0] + 1, currentIndex[1] -1, currentIndex[2]}*/
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

std::tuple<LinkedCellContainer::Border, int> LinkedCellContainer::getBorder(const std::array<int, 3> &currentIndexes, int d) {
    // y value is zero --> upper border
    if (currentIndexes[1] <= 0 && d == 1) {
        return std::make_tuple(border[2], 2);
    }
    // x value is zero --> left border
    if (currentIndexes[0] <= 0 && d == 0) {
        return std::make_tuple(border[0], 0);
    }
    // y value is max --> lower border
    if (currentIndexes[1] >= dim[1] - 1 && d == 1) {
        return std::make_tuple(border[3], 3);
    }
    // x value is max --> right border
    if (currentIndexes[0] >= dim[0] - 1 && d == 0) {
        return std::make_tuple(border[1], 1);
    }
    // z value is zero --> front
    if (currentIndexes[2] <= 0 && d == 2) {
        return std::make_tuple(border[4], 4);
    }
    // z value is max --> back
    if (currentIndexes[2] >= dim[2] - 1 && d == 2) {
        return std::make_tuple(border[5], 5);
    }
    // not a border
    return std::make_tuple(none, -1);
}

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


[[nodiscard]] size_t LinkedCellContainer::size() const noexcept {
    return particles.size();
}

void LinkedCellContainer::reserve(size_t size) {
    particles.reserve(size);
}

void LinkedCellContainer::emplace_back(Particle&& part) {
    particles.emplace_back(part);
}

void LinkedCellContainer::emplace_back(Particle& part) {
    particles.emplace_back(part);
}

void LinkedCellContainer::emplace_back(const std::array<double, 3>& x, const std::array<double, 3>& v, double m, int t) {
    particles.emplace_back(x,v,m,t);
}

void LinkedCellContainer::push_back(const Particle&& p) {
    particles.push_back(p);
}

void LinkedCellContainer::push_back(const Particle& p) {
    particles.push_back(p);
}


std::vector<Particle>::iterator LinkedCellContainer::begin() {
    return particles.begin();
}

std::vector<Particle>::iterator LinkedCellContainer::end() {
    return particles.end();
}

std::vector<Particle>::const_iterator LinkedCellContainer::begin() const {
    return particles.begin();
}

std::vector<Particle>::const_iterator LinkedCellContainer::end() const {
    return particles.end();
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

PairIterator LinkedCellContainer::pair_begin() {
    // ++ to skip pair (0,0)
    return {particles, 0, 1};
}

PairIterator LinkedCellContainer::pair_end() {
    return {particles, particles.size(), particles.size()};
}






