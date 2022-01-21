
#include <iostream>
#include <utility>
#include "LinkedCellContainer.h"


LinkedCellContainer::LinkedCellContainer(double Xv, double Yv, double Zv, double rCutV, std::array<Border, 6> borderV, std::array<double,3> g,
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
					switch (strategy) {
						case primitiveX:
							grid[i].setNeighbors2DInX();
							break;
						default:
							grid[i].setNeighbors2DInY();
							break;
					}
					grid[i].setIsBorderCell(currentIndexes[0] == 0 || currentIndexes[0] == dim[0] - 1 ||
						currentIndexes[1] == 0 || currentIndexes[1] == dim[1] - 1);
				} else {
					switch (strategy) {
						case primitiveX:
							grid[i].setNeighbors3DInX();
							break;
						case primitiveY:
							grid[i].setNeighbors3DInY();
							break;
						case primitiveZ:
							grid[i].setNeighbors3DInZ();
							break;
						default:
							grid[i].setNeighbors3DInY();
							break;
					}
					grid[i].setIsBorderCell(currentIndexes[0] == 0 || currentIndexes[0] == dim[0] - 1 ||
						currentIndexes[1] == 0 || currentIndexes[1] == dim[1] - 1 ||
						currentIndexes[2] == 0 || currentIndexes[2] == dim[2] - 1);
				}
				i++;
			}
		}
	}
	// Create the vector with indices for the threads
	if (strategy == primitiveX) {
		// get the greatest dimension
		// X is the greatest dimension
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
	} else if (strategy == primitiveY) {
		// Y is the greatest dimension
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
	else if (strategy == primitiveZ) {
		// We are in 2D, and we split along the Y axis since there is no Z axis
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
	} else if (strategy == subDomain) {
		int numThreads{};
		// get the greatest dimension
		// X is the greatest dimension
		if (dim[0] == std::max({dim[0], dim[1], dim[2]})) {
			// If domain size is smaller than our available threads, we use domain size as number of threads
			numThreads = std::min(omp_get_max_threads(), (dim[0] / 2 == 0 ? 1 : dim[0] / 2));
			// Length of every subdomain
			int len = dim[0] / numThreads;
			// Number of subdomains
			int num = static_cast<int>(std::ceil(static_cast<double>(dim[0]) / len));
			// Creating the vector
			subDomainVector = std::vector<SubDomain>(num);
			// Initializing the vector
			for (int j = 0; j < num; ++j) {
				auto subDomain = SubDomain{is2D(), 0};
				subDomain.setMinCoord({j * len, 0, 0});
				subDomain.setMaxCoord({std::min(dim[0], j * len + len) - 1, dim[1] - 1, dim[2] - 1});
				for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
					for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
						for (currentIndexes[0] = j * len; currentIndexes[0] < std::min(dim[0], j * len + len);
						     ++currentIndexes[0]) {
							subDomain.addIndex(currentIndexes,
							                   index({currentIndexes[0], currentIndexes[1], currentIndexes[2]}),
							                   grid[index({currentIndexes[0], currentIndexes[1], currentIndexes[2]})]
							);
						}
					}
				}
				subDomainVector[j] = subDomain;
			}
		}
			// Y is the greatest dimension
		else if (dim[1] == std::max({dim[0], dim[1], dim[2]})) {
			// If domain size is smaller than our available threads, we use domain size as number of threads
			numThreads = std::min(omp_get_max_threads(), (dim[1] / 2 == 0 ? 1 : dim[1] / 2));
			// Length of every subdomain
			int len = dim[1] / numThreads;
			// Number of subdomains
			int num = static_cast<int>(std::ceil(static_cast<double>(dim[1]) / len));
			// Creating the vector
			subDomainVector = std::vector<SubDomain>(num);
			// Initializing the vector
			for (int j = 0; j < num; ++j) {
				auto subDomain = SubDomain{is2D(), 1};
				subDomain.setMinCoord({0, j * len, 0});
				subDomain.setMaxCoord({dim[0] - 1, std::min(dim[1], j * len + len) - 1, dim[2] - 1});

				for (currentIndexes[2] = 0; currentIndexes[2] < dim[2]; ++currentIndexes[2]) {
					for (currentIndexes[1] = j * len; currentIndexes[1] < std::min(dim[1], j * len + len);
					     ++currentIndexes[1]) {
						for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
							subDomain.addIndex(currentIndexes,
							                   index({currentIndexes[0], currentIndexes[1], currentIndexes[2]}),
							                   grid[index({currentIndexes[0], currentIndexes[1], currentIndexes[2]})]
							);
						}
					}
				}
				subDomainVector[j] = subDomain;
			}
		}
			// Z is the greatest dimension
		else if (dim[2] == std::max({dim[0], dim[1], dim[2]})) {
			// If domain size is smaller than our available threads, we use domain size as number of threads
			numThreads = std::min(omp_get_max_threads(), (dim[2] / 2 == 0 ? 1 : dim[2] / 2));
			// Length of every subdomain
			int len = dim[2] / numThreads;
			// Number of subdomains
			int num = static_cast<int>(std::ceil(static_cast<double>(dim[2]) / len));
			// Creating the vector
			subDomainVector = std::vector<SubDomain>(num);
			// Initializing the vector
			for (int j = 0; j < num; ++j) {
				auto subDomain = SubDomain{is2D(), 2};
				subDomain.setMinCoord({0, 0, j * len});
				subDomain.setMaxCoord({dim[0] - 1, dim[1] - 1, std::min(dim[2], j * len + len) - 1});
				for (currentIndexes[2] = j * len; currentIndexes[2] < std::min(dim[2], j * len + len);
				     ++currentIndexes[2]) {
					for (currentIndexes[1] = 0; currentIndexes[1] < dim[1]; ++currentIndexes[1]) {
						for (currentIndexes[0] = 0; currentIndexes[0] < dim[0]; ++currentIndexes[0]) {
							subDomain.addIndex(currentIndexes,
							                   index({currentIndexes[0], currentIndexes[1], currentIndexes[2]}),
							                   grid[index({currentIndexes[0], currentIndexes[1], currentIndexes[2]})]
							);
						}
					}
				}

				subDomainVector[j] = subDomain;
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

void LinkedCellContainer::SubDomain::addIndex(const std::array<int, 3> &indexArray, int index, Cell &cell) {
	const auto[x_min, y_min, z_min] = minCoord;
	const auto[x_max, y_max, z_max] = maxCoord;
	if (is2D) {
		// we have a left border cell
		if ((axis == 0 && indexArray[axis] == x_min) || (axis == 1 && indexArray[axis] == y_min) ||
			(axis == 0 && indexArray[axis] == x_max) || (axis == 1 && indexArray[axis] == y_max)) {
			borderCellIndices.push_back(index);
			cell.setAllNeighbors2D();
		}
			// we have a not-border cell
		else {
			cellIndices.push_back(index);
		}
	}
		// We are in 3D
	else {
		// we have a left border cell
		if ((axis == 0 && indexArray[axis] == x_min) || (axis == 1 && indexArray[axis] == y_min)
			|| (axis == 2 && indexArray[axis] == z_min) || (axis == 0 && indexArray[axis] == x_max) || (axis == 1 && indexArray[axis] == y_max)
			|| (axis == 2 && indexArray[axis] == z_max)) {
			borderCellIndices.push_back(index);
			cell.setAllNeighbors3D();
		}
			// we have a not-border cell
		else {
			cellIndices.push_back(index);
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
    return lenDim[2] < 1.000001;
}

const std::array<int, 3> &LinkedCellContainer::getDim() const {
	return dim;
}

int LinkedCellContainer::dimensions() {
    //return dim[2] == 1 ? 2 : 3;
	return lenDim[2] < 1.000001 ? 2 : 3;
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

std::array<double,3> LinkedCellContainer::getG() const {
    return g;
}

void LinkedCellContainer::setG(std::array<double,3> grav) {
    LinkedCellContainer::g = grav;
}

const std::vector<std::vector<int>> &LinkedCellContainer::getIndicesThreadVector() const {
	return indicesThreadVector;
}

void LinkedCellContainer::setIndicesThreadVector(const std::vector<std::vector<int>> &vec) {
    LinkedCellContainer::indicesThreadVector = vec;
}

int LinkedCellContainer::getThreadOffset() const {
	return threadOffset;
}

void LinkedCellContainer::setThreadOffset(int offset) {
    LinkedCellContainer::threadOffset = offset;
}

const std::vector<int> &LinkedCellContainer::getResidualThreadVector() const {
	return residualThreadVector;
}

void LinkedCellContainer::setResidualThreadVector(const std::vector<int> &vec) {
    LinkedCellContainer::residualThreadVector = vec;
}

LinkedCellContainer::Strategy LinkedCellContainer::getStrategy() const {
	return strategy;
}

void LinkedCellContainer::setStrategy(LinkedCellContainer::Strategy s) {
    LinkedCellContainer::strategy = s;
}

const std::vector<LinkedCellContainer::SubDomain> &LinkedCellContainer::getSubDomainVector() const {
	return subDomainVector;
}

void LinkedCellContainer::setSubDomainVector(const std::vector<SubDomain> &SubDomainVector) {
	subDomainVector = SubDomainVector;
}

const std::vector<int> &LinkedCellContainer::SubDomain::getCellIndices() const {
	return cellIndices;
}

void LinkedCellContainer::SubDomain::setCellIndices(const std::vector<int> &CellIndices) {
	cellIndices = CellIndices;
}

const std::array<int, 3> &LinkedCellContainer::SubDomain::getMinCoord() const {
	return minCoord;
}

void LinkedCellContainer::SubDomain::setMinCoord(const std::array<int, 3> &MinCoord) {
	minCoord = MinCoord;
}

const std::array<int, 3> &LinkedCellContainer::SubDomain::getMaxCoord() const {
	return maxCoord;
}

void LinkedCellContainer::SubDomain::setMaxCoord(const std::array<int, 3> &MaxCoord) {
	maxCoord = MaxCoord;
}

LinkedCellContainer::SubDomain::SubDomain(std::vector<int> CellIndices,
                                          const std::array<int, 3> &MinCoord,
                                          const std::array<int, 3> &MaxCoord)
	: cellIndices(std::move(CellIndices)), minCoord(MinCoord), maxCoord(MaxCoord) {}

bool LinkedCellContainer::SubDomain::isIs2D() const {
	return is2D;
}

void LinkedCellContainer::SubDomain::setIs2D(bool is_2_d) {
	is2D = is_2_d;
}

bool LinkedCellContainer::SubDomain::isInSubdomain(const std::array<int, 3> &currentIndex) const {
	const auto[x_min, y_min, z_min] = minCoord;
	const auto[x_max, y_max, z_max] = maxCoord;

	return currentIndex[0] >= x_min && currentIndex[0] <= x_max && currentIndex[1] >= y_min && currentIndex[1] <= y_max
		&& currentIndex[2] >= z_min && currentIndex[2] <= z_max;
}

LinkedCellContainer::SubDomain::SubDomain(bool is2D, int axis) : is2D{is2D}, axis{axis} {}

const std::vector<int> &LinkedCellContainer::SubDomain::getBorderCellIndices() const {
	return borderCellIndices;
}

LinkedCellContainer::SubDomain::SubDomain() = default;
