#pragma once

#include <vector>
#include <cmath>
#include <functional>
#include <spdlog/spdlog.h>
#include <omp.h>

#include "Particle.h"
#include "ParticleContainer.h"
#include "Cell.h"

class LinkedCellContainer : public ParticleContainer {
public:

	struct SubDomain {

		SubDomain();

		explicit SubDomain(bool is2D, int axis);

		SubDomain(std::vector<int> CellIndices,
		          const std::array<int, 3> &MinCoord,
		          const std::array<int, 3> &MaxCoord);

		[[nodiscard]] const std::vector<int> &getCellIndices() const;

		void setCellIndices(const std::vector<int> &CellIndices);

		[[nodiscard]] const std::array<int, 3> &getMinCoord() const;

		void setMinCoord(const std::array<int, 3> &MinCoord);

		[[nodiscard]] const std::array<int, 3> &getMaxCoord() const;

		void setMaxCoord(const std::array<int, 3> &MaxCoord);

		[[nodiscard]] bool isIs2D() const;

		void setIs2D(bool is_2_d);

		[[nodiscard]] const std::vector<int> &getBorderCellIndices() const;

		/**
		 * Adds a cell to the subDomain
		 * @param indexArray the index of the cell (x, y, z)
		 * @param index the number index of the cell
		 * @param cell the actual cell
		 */
		void addIndex(const std::array<int, 3> & indexArray, int index, Cell & cell);

		/**
		 * For checking if an index is in a specific subDomain
		 * @param currentIndex the current index (x, y, z)
		 * @return true if it is in the subDomain, false if it is not
		 */
		[[nodiscard]] bool isInSubdomain(const std::array<int, 3> & currentIndex) const;

	private:
		/**
		 * The cell indices this subdomain has to work through that are not border cells
		 */
		std::vector<int> cellIndices{};

		/**
		 * The border cell indices this subdomain has to work through
		 */
		std::vector<int> borderCellIndices{};

		/**
		 * Coordinate where the subdomain begins
		 */
		std::array<int, 3> minCoord{};

		/**
		 * Coordinate where the subdomain ends
		 */
		std::array<int, 3> maxCoord{};

		bool is2D{};

		/**
		 * The axis the subDomains are parted
		 * 0 = X, 1 = Y, 2 = Z
		 */
		int axis;
	};


	enum Border { outflow, periodic, reflective, none };
	/**
	 * primitive(X|Y|Z): we split domain in lines and do two run partitions:
	 * subDomain: we split the domain in subDomains
	 * serial: we have no parallelization technique
	 */
	enum Strategy { primitiveX, primitiveY, primitiveZ, subDomain, serial };

	/***********************************************************************/

	/**
	 * Normal Constructor, which creates a grid with X*Y*Z elements
	 * @param Xv length of X-Axis of the array == the length of the domain
	 * @param Yv length of Y-Axis of the array == the length of the domain
	 * @param Zv length of Z-Axis of the array == the length of the domain
	 * @param rCutV the r_cut value
	 * @param borderV the border types of the 6 (3D) or 4 (2D) borders
	 * @param g the gravity that applies to the particles
	 * @param strategy the multi threading strategy that should be used
	 */
	LinkedCellContainer(double Xv,
	                    double Yv,
	                    double Zv,
	                    double rCutV,
	                    std::array<Border, 6> borderV = std::array<Border, 6>{
		                    outflow, outflow, outflow, outflow, outflow, outflow},
                        std::array<double,3> g = std::array<double,3>{0.,0.,0.},
	                    Strategy strategy = serial);

	/**
	 * Default constructor
	 */
	LinkedCellContainer() = default;

	/**
	 * @brief creates the cell grid
	 */
	void setup() override;

	/**
	 * @brief performs cleanup, deletes invalid Particles
	 */
	void cleanup() override;

	[[nodiscard]] size_t size() const noexcept override;

	void reserve(size_t) override;

	void emplace_back(Particle &&part) override;

	void emplace_back(Particle &part) override;

	void emplace_back(const std::array<double, 3> &x, const std::array<double, 3> &v, double m, int t) override;

	void push_back(const Particle &&p) override;

	void push_back(const Particle &p) override;

	std::vector<Particle>::iterator begin() override;

	std::vector<Particle>::iterator end() override;

	[[nodiscard]] std::vector<Particle>::const_iterator begin() const override;

	[[nodiscard]] std::vector<Particle>::const_iterator end() const override;

	PairIterator pair_begin() override;

	PairIterator pair_end() override;

	/**
	 * @brief Provides the iterator for single particles at the start of the collection
	 * @return iterator
	 */
	std::vector<Cell>::iterator begin_cell();

	/**
	 * @brief Provides the iterator for single particles at the end of the collection
	 * @return iterator
	 */
	std::vector<Cell>::iterator end_cell();

	/**
	 * @brief Provides the _const_ iterator for single Cells at the start of the collection
	 * @return const iterator
	 */
	[[nodiscard]] std::vector<Cell>::const_iterator begin_cell() const;

	/**
	 * @brief Provides the _const_ iterator for single Cells at the end of the collection
	 * @return const iterator
	 */
	[[nodiscard]] std::vector<Cell>::const_iterator end_cell() const;

	/**
	 * Calculates whether the neighbor is in the range by calculating the longest distance of the middle point
	 * of the neighbor cell to its edges and then calculating the distance between the middle point and the particle
	 * and decide whether it should be considered.
	 * If the distance is higher than the middle point distance + rCut, then it will not be considered
	 * @param p the current particle
	 * @param neighbor the neighbor cell index
	 * @return true if it is in range, false if not
	 * \note{Not used, since it did not affect our calculations much}
	 */
	bool inline isNeighborInRange(const Particle *p, const std::array<int, 3> &neighbor);

	/**
	 * Gets the border and the index of the border
	 * 0: LEFT, 1: RIGHT, 2: UP, 3: DOWN, 4: FRONT, 5: BACK
	 * @param currentIndexes the index of the current cell
	 * @param d the axis we are currently in
	 * @return
	 */
	[[nodiscard]] std::tuple<Border, int> getBorders(const std::array<int, 3> &currentIndexes, int d) const;

	/**
	 * Returns a vector of neighbor indices depending on the border it should be mirrored at
	 * @param currentIndex the index of the current cell
	 * @param borders the borders the current index is at
	 * @return a vector of indices of the neighbor
	 * \note{This method will not be used anymore, since it is faster to use the neighbors generated by the
	 * getNeighbors method}
	 */
	std::vector<std::array<int, 3>> getPerNeighbors(const std::array<int, 3> &currentIndex);

	/**
	 * Calculates the distance between a position X a border
	 * @param X the position
	 * @param bord the specific border ([0;5])
	 * @return the distance
	 */
	[[nodiscard]] inline double getDistance(const std::array<double, 3> &X, int bord) const;

	/**
	 * Returns the index in the 1D Grid container
	 * @param currentIndexes the current indexes in the three dimensions
	 * @return The index
	 */
	[[nodiscard]] inline int index(const std::array<int, 3> &currentIndexes) const;

	/**
	 * Calculates whether the neighbor cell is included as periodic neighbor
	 * @param neighbor the index of the neighbor
	 * @return true if it counts as periodic neighbor, false if not
	 */
	[[nodiscard]] bool isPeriodic(const std::array<int, 3> &neighbor) const;

	/***** Getters *****/

	[[nodiscard]] const std::vector<Cell> &getGrid() const;

	[[nodiscard]] double getRCut() const;

	[[nodiscard]] const std::array<int, 3> &getDim() const;

	[[nodiscard]] const std::array<double, 3> &getLenDim() const;

	bool is2D();

	int dimensions() override;

	/***** Setters *****/
	void setGrid(const std::vector<Cell> &grid);

	void setDim(const std::array<int, 3> &dim);

	void setRCut(double rCutV);

	void setLenDim(const std::array<double, 3> &lenDim);

	[[nodiscard]] const std::vector<Particle> &getParticles() const;

	void setParticles(const std::vector<Particle> &particles);

	[[nodiscard]] const std::array<Border, 6> &getBorder() const;

	void setBorder(const std::array<Border, 6> &border);

    [[nodiscard]] std::array<double,3> getG() const;

    void setG(std::array<double,3> g);

	[[nodiscard]] const std::vector<std::vector<int>> &getIndicesThreadVector() const;

	void setIndicesThreadVector(const std::vector<std::vector<int>> &indicesThreadVector);

	[[nodiscard]] int getThreadOffset() const;

	void setThreadOffset(int threadOffset);

	[[nodiscard]] const std::vector<int> &getResidualThreadVector() const;

	void setResidualThreadVector(const std::vector<int> &residualThreadVector);

	[[nodiscard]] Strategy getStrategy() const;

	void setStrategy(Strategy strategy);

	[[nodiscard]] const std::vector<SubDomain> &getSubDomainVector() const;

	void setSubDomainVector(const std::vector<SubDomain> &SubDomainVector);

	/**
	 * Has X * Y * Z many elements
	 * Vector where we save all our cells, which have the pointers to our particles
	 */
	std::vector<Cell> grid;
	/**
	 * Vector that saves all the instantiations of our particles
	 */
	std::vector<Particle> particles;

private:
	/**
	 * The array describes the length of the respective dimensions
	 * dim[0] = X, dim[1] = Y, dim[2] = Z
	 */
	std::array<int, 3> dim{};
	/**
	 * The array describes how long the domain of the respective dimensions should've been
	 * lenDim[0] = X, lenDim[1] = Y, lenDim[2] = Z
	 */
	std::array<double, 3> lenDim{};
	double rCut{};

	/**
	 * This array describes the borders of our domain
	 */
	std::array<Border, 6> border{outflow};

    /*
     * The gravitational force that applies to the domain
     */
    std::array<double,3> g{};

	/**
	 * Signals what multi threading strategy to use
	 */
	Strategy strategy{};

	/**
	 * This vector depicts which indices each thread has to work with in the first iteration
	 * The length is the number of threads
	 */
	std::vector<std::vector<int>> indicesThreadVector;

	/**
	 * Depicts how much offset the indices of the threads have in the second iteration
	 */
	int threadOffset{};

	/**
	 * If dimension is an uneven number, we have a residual thread vector
	 */
	std::vector<int> residualThreadVector{};

	/**
	 * If the strategy is 'subDomain' then we need a vector to keep up with the subDomains
	 */
	std::vector<SubDomain> subDomainVector{};

};

double LinkedCellContainer::getDistance(const std::array<double, 3> &X, int bord) const {
	switch (bord) {
		// LEFT
		case 0: return X[0];
			// RIGHT
		case 1: return lenDim[0] - X[0];
			// UPPER
		case 2: return X[1];
			// LOWER
		case 3: return lenDim[1] - X[1];
			// FRONT
		case 4: return X[2];
			// BACK
		case 5: return lenDim[2] - X[2];
			// INVALID CASE
		default: return -1;
	}
}

bool inline LinkedCellContainer::isNeighborInRange(const Particle *p, const std::array<int, 3> &neighbor) {
	// the max range from middle point of neighbor to its edges
	double midPointEdgeRange{};
	for (int d = 0; (dim[2] == 1) ? d < 2 : d < 3; ++d) {
		midPointEdgeRange += static_cast<double>(lenDim[d]) / static_cast<double>(dim[d]) *
			lenDim[d] / dim[d];
	}
	midPointEdgeRange = sqrt(midPointEdgeRange);

	// position of the neighborCell middle point
	std::array<double, 3> neighborPos{};
	for (int d = 0; (dim[2] == 1) ? d < 2 : d < 3; ++d) {
		neighborPos[d] = neighbor[d] * (lenDim[d] / dim[d]);
	}

	// Distance between these points
	double distance{};
	for (int d = 0; (dim[2] == 1) ? d < 2 : d < 3; ++d) {
		distance += (neighborPos[d] - p->getX()[d]) * (neighborPos[d] - p->getX()[d]);
	}
	distance = sqrt(distance);

	return distance <= midPointEdgeRange + rCut;
}

int LinkedCellContainer::index(const std::array<int, 3> &currentIndexes) const {
	return currentIndexes[0] + dim[0] * (currentIndexes[1] + dim[1] * currentIndexes[2]);
}