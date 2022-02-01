#include <iostream>
#include "LinkedCell.h"

namespace calculator {

void LinkedCell::calcFWithinCell(Cell &cell) {
	for (auto it = cell.begin(); it != cell.end(); ++it) {
		for (auto it2 = it; it2 != cell.end(); ++it2) {
			if (*it != *it2) {
				calcLJForce(*it, *it2, true);
			}
		}
	}
}



void LinkedCell::calcX(ParticleContainer &container) const {
	auto &gridLC = static_cast<LinkedCellContainer &>(container);

	if (gridLC.getStrategy() == LinkedCellContainer::serial) {
		for (auto &curCell : gridLC.getGrid()) { // Loop through every cell
			// If a cell is empty, skip it
			if (curCell.getParticles().empty()) continue;
			moveAndCalcXInCell(gridLC, curCell);
		}
	}
	else {
#ifdef _OPENMP
#pragma omp parallel for default(none) schedule(guided) shared(gridLC)
#endif //_OPENMP
		for (auto &curCell : gridLC.getGrid()) { // Loop through every cell
			// If a cell is empty, skip it
			if (curCell.getParticles().empty()) continue;
			moveAndCalcXInCell(gridLC, curCell);
		}
	}
}

void LinkedCell::moveParticles(LinkedCellContainer &grid) {
	std::array<int, 3> currentIndexes{};
	for (auto &curCell : grid.grid) {
		currentIndexes = curCell.getIndex();

		// checks if it is a border cell
		if (currentIndexes[0] == 0 || currentIndexes[0] == grid.getDim()[0] - 1 ||
			currentIndexes[1] == 0 || currentIndexes[1] == grid.getDim()[1] - 1 ||
			currentIndexes[2] == 0 || currentIndexes[2] == grid.getDim()[2] - 1) {

			for (auto &it : curCell) {

				// Checks whether any particle has crossed the boundaries
				for (int d = 0; d < 3; ++d) {
					if (it->getX()[d] < 0) {
						// outflow, removing the particle
						if (std::get<0>(grid.getBorders(currentIndexes, d)) ==
							LinkedCellContainer::outflow) {
							spdlog::info("Removing Particle");
							it->valid = false;
							break;
						}
							// periodic
						else if (std::get<0>(grid.getBorders(currentIndexes, d)) ==
							LinkedCellContainer::periodic) {
							// set X to the opposite site
							spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
							             it->getX()[0], it->getX()[1], it->getX()[2],
							             grid.getLenDim()[d] + it->getX()[d]);
							it->setX(d, grid.getLenDim()[d] + it->getX()[d]);
						}
					} else if (it->getX()[d] >= grid.getLenDim()[d]) {
						// outflow, removing the particle
						if (std::get<0>(grid.getBorders(currentIndexes, d)) ==
							LinkedCellContainer::outflow) {
							spdlog::info("Removing Particle");
							it->valid = false;
							break;
						}
							// periodic
						else if (std::get<0>(grid.getBorders(currentIndexes, d)) ==
							LinkedCellContainer::periodic) {
							// set X to the opposite site
							spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
							             it->getX()[0], it->getX()[1], it->getX()[2],
							             it->getX()[d] - grid.getLenDim()[d]);
							it->setX(d, it->getX()[d] - grid.getLenDim()[d]);
						}
					}
				}
			}
		}
	}
}

void LinkedCell::calcFSingleNeighbor(LinkedCellContainer &grid, const std::array<int, 3> &neighbors,
                                     Particle *p, bool newton) {
	// Loops through every particle of the neighbor
	for (auto &p_other : grid.grid[grid.index(neighbors)]) {
		if (p->immovable && p_other->immovable) continue;
		calcLJForce(p, p_other, newton);
	}
}

void LinkedCell::reflectiveBoundary(LinkedCellContainer &grid, const std::array<int, 3> &currentIndexes) const {
	// saves the reflective borders of the current cell
	std::vector<int> reflBorder{};
	// go through all three or two axis and acquire the borders of currentIndex that are reflective
	for (int d = 0; d < (grid.is2D() ? 2 : 3); ++d) {
		const auto[bordType, bord] = grid.getBorders(currentIndexes, d);
		if (bordType == LinkedCellContainer::reflective) {
			reflBorder.push_back(bord);
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
	if (!reflBorder.empty()) {
		for (auto &p : grid.grid[grid.index(currentIndexes)]) {

			for (int bord : reflBorder) {
				double r = grid.getDistance(p->getX(), bord);
				// reflect distance depending on different sigma for each particle
				double sig = sigmaTable[p->getSEIndex()][p->getSEIndex()];
				double reflectDistance = SIXTH_ROOT_OF_TWO * sig;
				if (r <= reflectDistance) {
					//double s = (sigma * sigma) / (r * r);

					double s = (sig * sig) / (r * r);
					s = s * s * s;
					auto force = -24 * epsilonTable[p->getSEIndex()][p->getSEIndex()] / r * s * (1 - 2 * s);

					auto newF{p->getF()};
					switch (bord) {
						case 0:newF[0] += force;
							break;
						case 1:newF[0] -= force;
							break;
						case 2:newF[1] += force;
							break;
						case 3:newF[1] -= force;
							break;
						case 4:newF[2] += force;
							break;
						case 5:newF[2] -= force;
							break;
						default:spdlog::critical("DEFAULT CASE SOMETHING WRONG ALARM");
					}
					p->setF(newF);
				}
			}
		}
	}
}

void LinkedCell::calcPerNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors, Particle *p,
                                  const std::array<double, 3> &mirror, bool newton) const {
	if (p->immovable) return;
	// Loop through the neighbors
	for (auto &p_other : grid.grid[grid.index(neighbors)]) {
		// This if is important if the domain only has one cell
		if (p != p_other) {
			auto mirroredX = p_other->getX() + mirror;
			double sqrd_dist = 0;
			for (int i = 0; i < DIM; i++) {
				sqrd_dist += LinkedCell::sqr(mirroredX[i] - p->getX()[i]);
			}
			if (sqrd_dist <= LinkedCell::sqr(rCut)) {
				std::array<double, 3> force{};
				if (smoothed && sqrd_dist > rl * rl) {
					double dist = sqrt(sqrd_dist);

					double sigma_pow_6 = std::pow(sigmaTable[p->getSEIndex()][p_other->getSEIndex()], 6);
					double dist_pow_6 = sqrd_dist * sqrd_dist * sqrd_dist;
					auto first_part =
						(rCut - dist) * (-(24 * sigma_pow_6) * epsilonTable[p->getSEIndex()][p_other->getSEIndex()])
							/ (dist_pow_6 * dist_pow_6 * sqrd_dist * std::pow(rCut - rl, 3));
					double second_part = rCut * rCut * (2 * sigma_pow_6 - dist_pow_6)
						+ rCut * (3 * rl - dist) * (dist_pow_6 - 2 * sigma_pow_6) +
						dist
							* (5 * rl * sigma_pow_6 - 2 * rl * dist_pow_6 - 3 * sigma_pow_6 * dist + dist_pow_6 * dist);
					force = (first_part * second_part) * (mirroredX - p->getX());
				} else {
					double s = sqr(sigmaTable[p->getSEIndex()][p_other->getSEIndex()]) / sqrd_dist;
					s = s * s * s; // s = sqr(s) * s
					double f = 24 * epsilonTable[p->getSEIndex()][p_other->getSEIndex()] * s / sqrd_dist * (1 - 2 * s);

					force = f * (mirroredX - p->getX());
				}

				p->setF(p->getF() + force);
				if (newton) {
					p_other->setF(p_other->getF() - force);
				}
			}
		}
	}
}

void LinkedCell::calcFWithNeighbors(LinkedCellContainer &grid,
                                    Particle *p,
                                    const std::array<int, 3> &neighbor,
                                    bool newton) {
	// Neighbor should be existing
	if (neighbor[0] < grid.getDim()[0] && neighbor[1] < grid.getDim()[1] &&
		neighbor[2] < grid.getDim()[2] && neighbor[0] >= 0 && neighbor[1] >= 0 &&
		neighbor[2] >= 0) {
		// Does not bring too much performance benefits
		// if (grid.isNeighborInRange(p, neighbor)) {
		calcFSingleNeighbor(grid, neighbor, p, newton);
		//}
	} else if (grid.isPeriodic(neighbor)) { // Else it is a neighbor out of boundary, check if it's a periodic neighbour
		// neighbor on the other side
		std::array<int, 3> neigh{(neighbor + grid.getDim()) % grid.getDim()};

		// the mirror we are adding so that the particle gets mirrored
		std::array<double, 3> mirror{};
		mirror = {
			neighbor[0] == -1 ? -grid.getLenDim()[0] : neighbor[0] == grid.getDim()[0] && grid.getDim()[0] != 1
			                                           ? grid.getLenDim()[0] : 0.0,

			neighbor[1] == -1 ? -grid.getLenDim()[1] : neighbor[1] == grid.getDim()[1] && grid.getDim()[1] != 1
			                                           ? grid.getLenDim()[1] : 0.0,

			neighbor[2] == -1 ? -grid.getLenDim()[2] : neighbor[2] == grid.getDim()[2] && grid.getDim()[2] != 1
			                                           ? grid.getLenDim()[2] : 0.0};

		LinkedCell::calcPerNeighbors(grid, neigh, p, mirror, newton);
	}
}

void LinkedCell::calcFCellSubdomain(Cell &curCell,
                                    LinkedCellContainer &grid,
                                    const LinkedCellContainer::SubDomain &subDomain) {
	for (auto &p : curCell) {
		// get all the neighbors
		for (const std::array<int, 3> &neighbors : grid.is2D() ? curCell.getNeighbors2D() : curCell.getNeighbors3D()) {
			// The neighbor is in the subdomain
			if (subDomain.isInSubdomain(neighbors)) {
				calcFWithNeighbors(grid, p, neighbors, true);
			} else {
				calcFWithNeighbors(grid, p, neighbors, false);
			}
		}

		for (const std::array<int, 3> &neighbors : grid.is2D() ? curCell.getRemainingNeighbors2D()
		                                                       : curCell.getRemainingNeighbors3D()) {
			if (!subDomain.isInSubdomain(neighbors)) {
				calcFWithNeighbors(grid, p, neighbors, false);
			}
		}
	}

	// Calculates the forces within a cell
	calcFWithinCell(curCell);
	auto currentIndexes = curCell.getIndex();
	// checks if it is a border cell, if yes also calculate border forces
	if (curCell.isBorderCell1()) {
		reflectiveBoundary(grid, currentIndexes);
	}
}

void LinkedCell::calcFCell(Cell &curCell, LinkedCellContainer &grid) {
	// Loop through every particle in this cell
	for (auto &p : curCell) {
		// Get all the neighbors of this cell
		for (const std::array<int, 3> &neighbors :
			(grid.is2D() ? curCell.getNeighbors2D() : curCell.getNeighbors3D())) {
			calcFWithNeighbors(grid, p, neighbors, true);
		}
	}

	// Calculates the forces within a cell
	calcFWithinCell(curCell);
	auto currentIndexes = curCell.getIndex();
	// checks if it is a border cell, if yes also calculate border forces
	if (curCell.isBorderCell1()) {
		reflectiveBoundary(grid, currentIndexes);
	}
}

void LinkedCell::calcF(ParticleContainer &container) {
	auto &grid = static_cast<LinkedCellContainer &>(container);
	// Switches between parallelization technique
	switch (grid.getStrategy()) {
		case LinkedCellContainer::primitiveX:
		case LinkedCellContainer::primitiveY:
		case LinkedCellContainer::primitiveZ:
#ifdef _OPENMP
#pragma omp parallel shared(grid) default(none)
#endif
		{
#ifdef _OPENMP
#pragma omp for schedule(dynamic)
#endif
			// First part
			for (size_t i = 0; i < grid.getIndicesThreadVector().size(); ++i) {
				for (int pos : grid.getIndicesThreadVector()[i]) {
					if (!grid.grid[pos].getParticles().empty()) {
						calcFCell(grid.grid[pos], grid);
					}
				}
			}
#ifdef _OPENMP
#pragma omp for schedule(dynamic) nowait // no wait since next line doesn't have to wait
#endif
			// Second part
			for (size_t i = 0; i < grid.getIndicesThreadVector().size(); ++i) {
				for (int pos : grid.getIndicesThreadVector()[i]) {
					if (!grid.grid[pos + grid.getThreadOffset()].getParticles().empty()) {
						calcFCell(grid.grid[pos + grid.getThreadOffset()], grid);
					}
				}
			}
#ifdef _OPENMP
#pragma omp single // single last remaining line
#endif
			{
				for (int pos : grid.getResidualThreadVector()) {
					if (!grid.grid[pos].getParticles().empty()) {
						calcFCell(grid.grid[pos], grid);
					}
				}
			};
		}; // end parallelization area
			break;

		case LinkedCellContainer::subDomain:
#ifdef _OPENMP
#pragma omp parallel shared(grid) default(none)
#endif
		{
#ifdef _OPENMP
#pragma omp for schedule(dynamic)
#endif
			for (auto &subDomain : grid.getSubDomainVector()) {
				// Normal cells
				for (int pos : subDomain.getCellIndices()) {
					if (!grid.grid[pos].getParticles().empty()) {
						calcFCell(grid.grid[pos], grid);
					}
				}
				// Border cells
				for (int pos : subDomain.getBorderCellIndices()) {
					if (!grid.grid[pos].getParticles().empty()) {
						calcFCellSubdomain(grid.grid[pos], grid, subDomain);
					}
				}
			}
		}; // end parallelization area
			break;

		case LinkedCellContainer::serial:
			for (auto &curCell : grid.grid) {
				calcFCell(curCell, grid);
			}
			break;
	}
}

void LinkedCell::harmonic_potential(Particle *p1, Particle *p2, double sqrd_dist, bool newton) const {
	double factor = SQR_ROOT_OF_TWO;
	double distance = sqrt(sqrd_dist);
	// if orthogonal neighbors
	if (abs(p1->getGridIndex()[0] - p2->getGridIndex()[0]) + abs(p1->getGridIndex()[1] - p2->getGridIndex()[1])
		+ abs(p1->getGridIndex()[2] - p2->getGridIndex()[2]) == 1) {
		factor = 1;
	}
	auto f = stiffnessConstant * (distance - factor * rZero) * (1. / distance) * (p2->getX() - p1->getX());
	p1->setF(p1->getF() + f);
	if (newton) {
		p2->setF(p2->getF() - f);
	}
}

bool LinkedCell::gridNeighbors(const std::array<int, 3> &index1, const std::array<int, 3> &index2) {
	return abs(index1[0] - index2[0]) <= 1 && abs(index1[1] - index2[1]) <= 1 && abs(index1[2] - index2[2]) <= 1;
}

void LinkedCell::setSigmaTable(const std::vector<std::vector<double>> &sT) {
	LinkedCell::sigmaTable = sT;
}

void LinkedCell::setEpsilonTable(const std::vector<std::vector<double>> &eT) {
	LinkedCell::epsilonTable = eT;
}

void LinkedCell::setMapping(std::vector<std::pair<int, std::pair<double, double>>> &map) {
	LinkedCell::mapping = map;
}

void LinkedCell::setRZero(double rZ) {
	LinkedCell::rZero = rZ;
}

void LinkedCell::setStiffnessConstant(double sC) {
	LinkedCell::stiffnessConstant = sC;
}

void LinkedCell::setMembrane(bool mem) {
	LinkedCell::membrane = mem;
}

std::string LinkedCell::toString() {
	return "LinkedCell";
}

void to_json(json &j, const LinkedCell &p) {
	j = nlohmann::json{{"mapping", p.mapping}};
}

void from_json(const nlohmann::json &j, LinkedCell &p) {
	j.at("mapping").get_to(p.mapping);
	//j.at("epsilonTable").get_to(p.epsilonTable);
}

}