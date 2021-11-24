#include "LinkedCellContainer.h"

std::vector<LinkedCellContainer::Cell> &LinkedCellContainer::getGrid() {
    return grid;
}

int LinkedCellContainer::getX() const {
    return X;
}

int LinkedCellContainer::getY() const {
    return Y;
}

int LinkedCellContainer::getZ() const {
    return Z;
}

double LinkedCellContainer::getRCut() const {
    return rCut;
}

void LinkedCellContainer::setGrid(const std::vector<LinkedCellContainer::Cell> &grid) {
    LinkedCellContainer::grid = grid;
}

void LinkedCellContainer::setX(int x) {
    X = x;
}

void LinkedCellContainer::setY(int y) {
    Y = y;
}

void LinkedCellContainer::setZ(int z) {
    Z = z;
}

void LinkedCellContainer::setRCut(double rCutV) {
    LinkedCellContainer::rCut = rCutV;
}
