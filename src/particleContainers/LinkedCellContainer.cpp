
#include <iostream>
#include "LinkedCellContainer.h"

LinkedCellContainer::LinkedCellContainer(double Xv, double Yv, double Zv, double rCutV, std::array<Border, 6> borderV,
                                         double g,
                                         Strategy strategy) :
    grid{std::vector<Cell>(static_cast<int>(std::floor(Xv / rCutV)) *
        static_cast<int>(std::floor(Yv / rCutV)) *
        (static_cast<int>(std::floor(Zv / rCutV)) == 0 ? 1 :
         static_cast<int>(std::floor(Zv / rCutV))))},
    dim{std::array<int, 3>{static_cast<int>(std::floor(Xv / rCutV)),
                           static_cast<int>(std::floor(Yv / rCutV)),
                           (static_cast<int>(std::floor(Zv / rCutV))) == 0 ? 1 :
                           static_cast<int>(std::floor(Zv / rCutV))}},
    lenDim{std::array<double, 3>{Xv, Yv, Zv}}, rCut{rCutV}, border{borderV}, g{g}, strategy{strategy} {
    // Initialize the grid
    int i{0};
    std::array<int, 3> currentIndexes{};
    // iterate through Z axis
    for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
        // iterate through the Y axis
        for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
            // iterate through the X axis
            for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                grid[i].setIndex(currentIndexes);
                if (is2D()) {
                    grid[i].setNeighbors2D();
                    grid[i].setIsBorderCell(currentIndexes[0] == 0 || currentIndexes[0] == dim[0] - 1 ||
                        currentIndexes[1] == 0 || currentIndexes[1] == dim[1] - 1);
                } else {
                    grid[i].setNeighbors3D();
                    grid[i].setIsBorderCell(currentIndexes[0] == 0 || currentIndexes[0] == dim[0] - 1 ||
                        currentIndexes[1] == 0 || currentIndexes[1] == dim[1] - 1 ||
                        currentIndexes[2] == 0 || currentIndexes[2] == dim[2] - 1);
                }
                i++;
            }
        }
    }
    // Create the vector with indices for the threads
    if (strategy == primitive) {
        // get the greatest dimension
        // X is the greatest dimension
        if (dim[0] == std::max({dim[0], dim[1], dim[2]})) {
            threadOffset = 1;
            indicesThreadVector = std::vector<std::vector<int>>(dim[0] / 2);
            for (currentIndexes[0] = 0; currentIndexes[0] < dim[0] / 2; ++currentIndexes[0]) {
                for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
                    for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
                        indicesThreadVector[currentIndexes[0]]
                            .push_back(index({currentIndexes[0] * 2, currentIndexes[1], currentIndexes[2]}));
                    }
                }
            }
            // We have an uneven dimension
            if (dim[0] % 2 == 1) {
                for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
                    for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
                        residualThreadVector
                            .push_back(index({dim[0] - 1, currentIndexes[1], currentIndexes[2]}));
                    }
                }
            }
        }
            // Y is the greatest dimension
        else if (dim[1] == std::max({dim[0], dim[1], dim[2]})) {
            indicesThreadVector = std::vector<std::vector<int>>(dim[1] / 2);
            threadOffset = dim[0];
            for (currentIndexes[1] = 0; currentIndexes[1] < dim[1] / 2; ++currentIndexes[1]) {
                for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
                    for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                        indicesThreadVector[currentIndexes[1]]
                            .push_back(index({currentIndexes[0], currentIndexes[1] * 2, currentIndexes[2]}));
                    }
                }
            }
            // We have an uneven dimension
            if (dim[1] % 2 == 1) {
                for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
                    for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                        residualThreadVector
                            .push_back(index({currentIndexes[0], dim[1] - 1, currentIndexes[2]}));
                    }
                }
            }
        }
            // Z is the greatest dimension
        else if (dim[2] == std::max({dim[0], dim[1], dim[2]})) {
            indicesThreadVector = std::vector<std::vector<int>>(dim[2] / 2);
            threadOffset = dim[0] * dim[1];
            for (currentIndexes[2] = 0; currentIndexes[2] < dim[2] / 2; ++currentIndexes[2]) {
                for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
                    for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                        indicesThreadVector[currentIndexes[2]]
                            .push_back(index({currentIndexes[0], currentIndexes[1], currentIndexes[2] * 2}));
                    }
                }
            }
            if (dim[2] % 2 == 1) {
                for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
                    for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                        residualThreadVector
                            .push_back(index({currentIndexes[0], currentIndexes[1], dim[2] - 1}));
                    }
                }
            }
        }
    } else if (strategy == primitiveFit) {
        // We are in 2D, and we split along the Y axis
        if (dim[2] == 1) {
            indicesThreadVector = std::vector<std::vector<int>>(dim[1] / 2);
            threadOffset = dim[0];
            for (currentIndexes[1] = 0; currentIndexes[1] < dim[1] / 2; ++currentIndexes[1]) {
                for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                    indicesThreadVector[currentIndexes[1]]
                        .push_back(index({currentIndexes[0], currentIndexes[1] * 2, 0}));
                }

            }
            // We have an uneven dimension
            if (dim[1] % 2 == 1) {
                for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                    residualThreadVector
                        .push_back(index({currentIndexes[0], dim[1] - 1, 0}));
                }

            }
        }
            // We are in 3D, and we split along the Z axis
        else {
            indicesThreadVector = std::vector<std::vector<int>>(dim[2] / 2);
            threadOffset = dim[0] * dim[1];
            for (currentIndexes[2] = 0; currentIndexes[2] < dim[2] / 2; ++currentIndexes[2]) {
                for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
                    for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                        indicesThreadVector[currentIndexes[2]]
                            .push_back(index({currentIndexes[0], currentIndexes[1], currentIndexes[2] * 2}));
                    }
                }
            }
            if (dim[2] % 2 == 1) {
                for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
                    for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
                        residualThreadVector
                            .push_back(index({currentIndexes[0], currentIndexes[1], dim[2] - 1}));
                    }
                }
            }
        }
    }
}

void LinkedCellContainer::setup() {
    for (auto &it : grid) {
        it.clear();
    }
    for (auto &p : particles) {
        if (p.valid) {
            std::array<int, 3> novelCellIndex{};
            for (int d = 0; d < 3; ++d) {
                novelCellIndex[d] = static_cast<int>(std::floor(
                    p.getX()[d] * getDim()[d] / getLenDim()[d]));
            }
            auto cellIndex = (*this).index(novelCellIndex);
            p.setOldF(p.getF());
            // here gravitational force is applied
            // p.setF({0., p.getM() * g, 0.});
            // set Force to baseForce + g*m
            p.applyBaseForceAndGrav(g);
            grid[cellIndex].emplace_back(&p);
        }
    }
}

void LinkedCellContainer::cleanup() {
    // use erase-remove idiom
    particles.erase(std::remove_if(particles.begin(),
                                   particles.end(), [](Particle &p) { return !p.valid; }), particles.end());
}

bool LinkedCellContainer::isPeriodic(const std::array<int, 3> &neighbor) const {
    bool result = true;
    for (int d = 0, b = 0; d < 3; ++d, b += 2) {
        if ((neighbor[d] < 0 && border[b] != periodic) || (neighbor[d] > dim[d] - 1 && border[b + 1] != periodic)) {
            return false;
        }
    }
    return result;
}

std::tuple<LinkedCellContainer::Border, int>
LinkedCellContainer::getBorders(const std::array<int, 3> &currentIndexes, int d) const {
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

const std::vector<Cell> &LinkedCellContainer::getGrid() const {
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

bool LinkedCellContainer::is2D() {
    return dim[2] == 1;
}

const std::array<int, 3> &LinkedCellContainer::getDim() const {
    return dim;
}

int LinkedCellContainer::dimensions() {
    return dim[2] == 1 ? 2 : 3;
}

void LinkedCellContainer::setDim(const std::array<int, 3> &dimV) {
    LinkedCellContainer::dim = dimV;
}

const std::array<double, 3> &LinkedCellContainer::getLenDim() const {
    return lenDim;
}

void LinkedCellContainer::setLenDim(const std::array<double, 3> &lenDimV) {
    LinkedCellContainer::lenDim = lenDimV;
}

[[nodiscard]] size_t LinkedCellContainer::size() const noexcept {
    return particles.size();
}

void LinkedCellContainer::reserve(size_t size) {
    particles.reserve(size);
}

void LinkedCellContainer::emplace_back(Particle &&part) {
    particles.emplace_back(part);
}

void LinkedCellContainer::emplace_back(Particle &part) {
    particles.emplace_back(part);
}

void
LinkedCellContainer::emplace_back(const std::array<double, 3> &x, const std::array<double, 3> &v, double m, int t) {
    particles.emplace_back(x, v, m, t);
}

void LinkedCellContainer::push_back(const Particle &&p) {
    particles.push_back(p);
}

void LinkedCellContainer::push_back(const Particle &p) {
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

const std::vector<Particle> &LinkedCellContainer::getParticles() const {
    return particles;
}

void LinkedCellContainer::setParticles(const std::vector<Particle> &particlesV) {
    LinkedCellContainer::particles = particlesV;
}

const std::array<LinkedCellContainer::Border, 6> &LinkedCellContainer::getBorder() const {
    return border;
}

void LinkedCellContainer::setBorder(const std::array<Border, 6> &borderV) {
    LinkedCellContainer::border = borderV;
}

std::vector<std::array<int, 3>>
LinkedCellContainer::getPerNeighbors(const std::array<int, 3> &currentIndex) {
    std::vector<std::array<int, 3>> neighbors{};
    // keeps track of the current index
    std::array<int, 3> cI{};
    // the actual values
    int x{}, y{}, z{};
    // traverse trough x-axis
    for (cI[0] = currentIndex[0] - 1; cI[0] <= currentIndex[0] + 1; ++cI[0]) {
        // traverse through y-axis
        for (cI[1] = currentIndex[1] - 1; cI[1] <= currentIndex[1] + 1; ++cI[1]) {
            // traverse through z-axis
            for (cI[2] = ((*this).is2D() ? 0 : currentIndex[2] - 1);
                 cI[2] <= ((*this).is2D() ? 0 : currentIndex[2] + 1); ++cI[2]) {

                // Not even out of bounds
                if (cI[0] >= 0 && cI[0] < dim[0] &&
                    cI[1] >= 0 && cI[1] < dim[1] &&
                    cI[2] >= 0 && cI[2] < dim[2]) {
                    continue;
                }

                x = cI[0];
                y = cI[1];
                z = cI[2];

                // Border left or right
                if ((cI[0] < 0 && border[0] == periodic) || (cI[0] >= dim[0] && border[1] == periodic)) {
                    x = (cI[0] + dim[0]) % dim[0];
                } else if (cI[0] < 0 || cI[0] >= dim[0]) {
                    continue;
                }
                // Border up and down
                if ((cI[1] < 0 && border[2] == periodic) || (cI[1] >= dim[1] && border[3] == periodic)) {
                    y = (cI[1] + dim[1]) % dim[1];
                } else if (cI[1] < 0 || cI[1] >= dim[1]) {
                    continue;
                }
                // Border front and back
                if ((cI[2] < 0 && border[4] == periodic) || (cI[2] >= dim[2] && border[5] == periodic)) {
                    z = (cI[2] + dim[2]) % dim[2];
                } else if (cI[2] < 0 || cI[2] >= dim[2]) {
                    continue;
                }
                neighbors.emplace_back(std::array<int, 3>{x, y, z});
            }
        }
    }
    return neighbors;
}

double LinkedCellContainer::getG() const {
    return g;
}

void LinkedCellContainer::setG(double g) {
    LinkedCellContainer::g = g;
}

const std::vector<std::vector<int>> &LinkedCellContainer::getIndicesThreadVector() const {
    return indicesThreadVector;
}

void LinkedCellContainer::setIndicesThreadVector(const std::vector<std::vector<int>> &indicesThreadVector) {
    LinkedCellContainer::indicesThreadVector = indicesThreadVector;
}

int LinkedCellContainer::getThreadOffset() const {
    return threadOffset;
}

void LinkedCellContainer::setThreadOffset(int threadOffset) {
    LinkedCellContainer::threadOffset = threadOffset;
}

const std::vector<int> &LinkedCellContainer::getResidualThreadVector() const {
    return residualThreadVector;
}

void LinkedCellContainer::setResidualThreadVector(const std::vector<int> &residualThreadVector) {
    LinkedCellContainer::residualThreadVector = residualThreadVector;
}

LinkedCellContainer::Strategy LinkedCellContainer::getStrategy() const {
    return strategy;
}

void LinkedCellContainer::setStrategy(LinkedCellContainer::Strategy strategy) {
    LinkedCellContainer::strategy = strategy;
}





