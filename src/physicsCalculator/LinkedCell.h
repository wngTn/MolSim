#pragma once

#include <iostream>
#include <omp.h>
#include "physicsCalculator/PhysicsCalc.h"
#include "physicsCalculator/LennardJones.h"
#include "particleContainers/LinkedCellContainer.h"
#include "utils/ArrayUtils.h"
#include "spdlog/spdlog.h"

#include "nlohmann/json.hpp"

namespace calculator {
class LinkedCell : public PhysicsCalc {

public:

	/**
	 * Constructor
	 * @param sigma sigma value
	 * @param epsilon epsilon value
	 * @param rCut rCut value
	 * @param smoothed whether smoothed lj should be used
	 * @param rl rl for smoothed lj force
	 */
	LinkedCell(double sigma, double epsilon, double rCut, bool smoothed = false, double rl = 0.0)
		: sigma(sigma), epsilon(epsilon),
		  rCut{rCut}, smoothed(smoothed), rl(rl) {
		epsilonTable = {{epsilon}};
		sigmaTable = {{sigma}};
	};

	std::string toString() override;

	/**
	* Returns the square of a number
	* @param x: the number
	*/
	template<typename T>
	static
	T sqr(T x);

	/**
	 * This method simply sets all Particles to invalid, if they cross any borders,
	 * or puts them on the other side, if there are cyclic borders
	 * @param grid the grid
	 * \note{deprecated}
	 */
	static void moveParticles(LinkedCellContainer &grid);

	/**
	 * override of the default calcX method, because here boundary conditions need to be applied
	 * @param grid the particle container (here a LinkedCellContainer, will be downcasted)
	 */
	void calcX(ParticleContainer &grid) const override;

	void calcF(ParticleContainer &grid) override;

	void setSigmaTable(const std::vector<std::vector<double>> &sigmaTable);

	void setEpsilonTable(const std::vector<std::vector<double>> &epsilonTable);

	void setMapping(std::vector<std::pair<int, std::pair<double, double>>> &mapping);

	void setRZero(double rZ);

	void setStiffnessConstant(double stiffnessConstant);

	void setMembrane(bool mem);

private:

	/**
	 * Method explicitly for calculating the forces within a cell
	 * @param cell the current cell
	 */
	void calcFWithinCell(Cell &cell);

	/**
	 * Calculates the actual lennard jones forces between two particles
	 * @param p1 the first particle
	 * @param p2 the second particle
	 * @param sqrd_dist the squared distance between the two particles
	 * @param newton whether newton's third law should be applied
	 */
	inline void ljforce(Particle *p1, Particle *p2, double sqrd_dist, bool newton = true) const;

	/**
	 * Calculates the smoothed lennard jones forces between two particles
	 * @param p1 the first particle
	 * @param p2 the second particle
	 * @param sqrd_dist the squared distance between the two particles
	 * @param newton whether newton's third law should be applied
	 */
	inline void ljforce_smoothed(Particle *p1, Particle *p2, double sqrd_dist, bool newton = true) const;

	/**
	 * Calculates how the lennard jones forces between two particles should be calculated
	 * @param p1 the first particle
	 * @param p2 the second particle
	 * @param newton whether newton's third law should be applied
	 */
	inline void calcLJForce(Particle *p1, Particle *p2, bool newton = true) const;

	/**
	 * Calculates the harmonic potential between two particles
	 * @param p1 the first particle
	 * @param p2 the second particle
	 * @param sqrd_dist the squared distance between them
	 * @param newton whether newton's third law should be applied
	 */
	void harmonic_potential(Particle *p1, Particle *p2, double sqrd_dist, bool newton = true) const;

	/**
	 * Calculates the reflective boundary forces in a single cell
	 * @param grid the domain
	 * @param currentIndex the cell
	 */
	void reflectiveBoundary(LinkedCellContainer &grid, const std::array<int, 3> &currentIndex) const;

	/**
	 * Calculates how the forces between a particle and a neighbor should be calculated
	 * @param grid the domain
	 * @param p the particle
	 * @param neighbor the neighbor
	 * @param newton whether newton's third law should be applied
	 */
	void calcFWithNeighbors(LinkedCellContainer &grid, Particle *p, const std::array<int, 3> &neighbor, bool newton);

	/**
	 * Calculates only the forces between a particle within a cell and its neighbor
	 * @param grid the domain
	 * @param neighbor the index of the neighbor
	 * @param p the particle
	 * @param newton whether newton's third law should be applied
	 */
	void calcFSingleNeighbor(LinkedCellContainer &grid, const std::array<int, 3> &neighbor,
	                         Particle *p, bool newton = true);


	/**
	 * Calculates the forces of a cell within a subDomain
	 * @param curCell the current cell
	 * @param grid the domain
	 * @param subDomain the subDomain the cell is in
	 */
	void calcFCellSubdomain(Cell &curCell, LinkedCellContainer &grid, const LinkedCellContainer::SubDomain &subDomain);

	/**
	 * Calculates the forces within a cell
	 * @param curCell the current cell
	 * @param grid the domain
	 */
	void calcFCell(Cell &curCell, LinkedCellContainer &grid);

	/**
	 * Calculates the new positions of the particles in a cell with regards of the borders
	 * @param gridLC the domain
	 * @param curCell the current cell
	 */
	inline void calcNewX(const LinkedCellContainer & gridLC, const Cell &curCell) const;

	/**
	 * Calculates the forces between p and its neighbor for the periodic boundary
	 * The bord tells this method how to calculate the distance
	 * @param grid the whole container
	 * @param neighbors the neighbor index
	 * @param p the particle we are calculating forces on
	 * @param mirror the array we should add so we get the mirrored position
	 */
	void calcPerNeighbors(LinkedCellContainer &grid, const std::array<int, 3> &neighbors,
	                      Particle *p, const std::array<double, 3> &mirror, bool newton = true) const;

	/**
	 * Checks whether two cells in the grid are neighbors
	 * @param index1 the index of the first cell
	 * @param index2 the index of the second cell
	 * @return true if they are neighbors, false if not
	 */
	static bool gridNeighbors(const std::array<int, 3> &index1, const std::array<int, 3> &index2);

	/// Default sigma value
	double sigma = 1;

	/// Default epsilon value
	double epsilon = 5;

	/// Default rCut value
	double rCut = 2.5 * epsilon;

	/// average bond length of molecule pair, used for membrane
	double rZero{};

	/// used for membrane
	double stiffnessConstant{};

	/// whether any membranes exist, used only to shortcut some checks
	bool membrane = false;

	/// whether smoothed LJ is used
	bool smoothed;

	/// r_l used for smoothed LJ
	double rl;

	/// The sigma table if two or more particle types exist
	std::vector<std::vector<double>> sigmaTable = {{0.0}};

	/// The epsilon table if two or more particle types exist
	std::vector<std::vector<double>> epsilonTable = {{0.0}};

	/// Mapping for sigma and epsilon values
	std::vector<std::pair<int, std::pair<double, double>>> mapping = {};

	using json = nlohmann::json;

	friend void to_json(json &, const LinkedCell &);

	friend void from_json(const json &, LinkedCell &p);

};


constexpr double SIXTH_ROOT_OF_TWO = 1.12246204830937298;

// copied from std::numbers which doesn't work on older compiler versions
constexpr double SQR_ROOT_OF_TWO = 1.414213562373095048801688724209698079L;

template<typename T>
T LinkedCell::sqr(T x) {
	return x * x;
}

void LinkedCell::ljforce(Particle *p1, Particle *p2, double sqrd_dist, bool newton) const {
	//double s = sqr(sigma) / sqrd_dist;
	double s = sqr(sigmaTable[p1->getSEIndex()][p2->getSEIndex()]) / sqrd_dist;
	s = s * s * s; // s = sqr(s) * s
	//  double f = 24 * epsilon * s / sqrd_dist * (1 - 2 * s);
	double f = 24 * epsilonTable[p1->getSEIndex()][p2->getSEIndex()] * s / sqrd_dist * (1 - 2 * s);

	auto force = f * (p2->getX() - p1->getX());

	p1->setF(p1->getF() + force);
	if (newton) {
		p2->setF(p2->getF() - force);
	}
}

void LinkedCell::ljforce_smoothed(Particle *p1, Particle *p2, double sqrd_dist, bool newton) const {
	double dist = sqrt(sqrd_dist);
	if (dist <= rl) {
		ljforce(p1, p2, sqrd_dist);
		return;
	} else { // dist < rCut checked b4 calling function
		double sigma_pow_6 = std::pow(sigmaTable[p1->getSEIndex()][p2->getSEIndex()], 6);
		double dist_pow_6 = sqrd_dist * sqrd_dist * sqrd_dist;
		auto first_part = (rCut - dist) * (-(24 * sigma_pow_6) * epsilonTable[p1->getSEIndex()][p2->getSEIndex()])
			/ (dist_pow_6 * dist_pow_6 * sqrd_dist * std::pow(rCut - rl, 3));
		double second_part =
			rCut * rCut * (2 * sigma_pow_6 - dist_pow_6) + rCut * (3 * rl - dist) * (dist_pow_6 - 2 * sigma_pow_6) +
				dist * (5 * rl * sigma_pow_6 - 2 * rl * dist_pow_6 - 3 * sigma_pow_6 * dist + dist_pow_6 * dist);
		auto force = (first_part * second_part) * (p2->getX() - p1->getX());
		p1->setF(p1->getF() + force);
		if (newton) {
			p2->setF(p2->getF() - force);
		}
	}
}

void LinkedCell::calcLJForce(Particle *p1, Particle *p2, bool newton) const {
	double sqrd_dist = 0;
	for (int i = 0; i < DIM; i++) {
		sqrd_dist += LinkedCell::sqr(p2->getX()[i] - p1->getX()[i]);
	}
	//LinkedCell::ljforce((p1), p2, sqrd_dist);
	if (!membrane || !p1->membrane || !p2->membrane) {
		if (sqrd_dist <= rCut * rCut && !(p1->immovable && p2->immovable)) {
			if (smoothed) {
				LinkedCell::ljforce_smoothed(p1, p2, sqrd_dist, newton);
			} else {
				LinkedCell::ljforce(p1, p2, sqrd_dist, newton);
			}
		}
	} else {
		if (sqrd_dist <= sqr(SIXTH_ROOT_OF_TWO * sigmaTable[(p1)->getSEIndex()][p2->getSEIndex()])) {
			if (smoothed) {
				LinkedCell::ljforce_smoothed(p1, p2, sqrd_dist, newton);
			} else {
				LinkedCell::ljforce(p1, p2, sqrd_dist, newton);
			}
		}
		if (gridNeighbors((p1)->getGridIndex(), p2->getGridIndex())) {
			LinkedCell::harmonic_potential(p1, p2, sqrd_dist, newton);
		}
	}
}

void LinkedCell::calcNewX(const LinkedCellContainer &gridLC, const Cell &curCell) const {
	auto currentIndexes = curCell.getIndex();
	// checks if it is a border cell
	if (curCell.isBorderCell1()) {
		for (auto &p : curCell) {
			auto newX = p->getX() + delta_t * (p->getV() + delta_t * 0.5 / p->getM() * p->getF());
			p->setX(newX);
			// Checks whether any particle has crossed the boundaries
			for (int d = 0; d < (gridLC.is2D() ? 2 : 3); ++d) {
				if (p->getX()[d] < 0) {
					// outflow, removing the particle
					if (std::get<0>(gridLC.getBorders(currentIndexes, d)) ==
						LinkedCellContainer::outflow) {
						spdlog::info("Removing Particle");
						p->valid = false;
						break;
					}
						// periodic
					else if (std::get<0>(gridLC.getBorders(currentIndexes, d)) == LinkedCellContainer::periodic) {
						// set X to the opposite site
						spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
						             p->getX()[0], p->getX()[1], p->getX()[2],
						             gridLC.getLenDim()[d] + p->getX()[d]);
						p->setX(d, gridLC.getLenDim()[d] + p->getX()[d]);
						p->setPassedPeriodic(d);
					}
				} else if (p->getX()[d] >= gridLC.getLenDim()[d]) {
					// outflow, removing the particle
					if (std::get<0>(gridLC.getBorders(currentIndexes, d)) == LinkedCellContainer::outflow) {
						spdlog::info("Removing Particle");
						p->valid = false;
						break;
					}
						// periodic
					else if (std::get<0>(gridLC.getBorders(currentIndexes, d)) == LinkedCellContainer::periodic) {
						// set X to the opposite site
						spdlog::info("Particle was at d: {} and position {} {} {} now at {}", d,
						             p->getX()[0], p->getX()[1], p->getX()[2],
						             p->getX()[d] - gridLC.getLenDim()[d]);
						p->setX(d, p->getX()[d] - gridLC.getLenDim()[d]);
						p->setPassedPeriodic(d);
					}
				}
			}
		}
	} else { // Not a border cell
		for (auto &p : curCell) {
			auto newX = p->getX() + delta_t * (p->getV() + delta_t * 0.5 / p->getM() * p->getF());
			p->setX(newX);
		}
	}
}
}





