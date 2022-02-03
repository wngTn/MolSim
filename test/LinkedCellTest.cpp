#include "gtest/gtest.h"

#include <Particle.h>
#include <physicsCalculator/LinkedCell.h>
#include "utils/ArrayUtils.h"
#include <omp.h>


/*
 * Tests whether the index method is correct
 */
TEST(LinkedCellTest, indexTest) {

	std::array<double, 3> dim = {3, 4, 1};
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{dim[0], dim[1], dim[2], 1.};
	std::array<int, 3> arr1 = {0, 0, 0};
	std::array<int, 3> arr2 = {0, 0, 1};
	std::array<int, 3> arr3 = {1, 2, 2};
	std::array<int, 3> arr4 = {1, 1, 1};
	std::array<int, 3> arr5 = {2, 3, 1};

	int res1 = linkedCellContainer.index(arr1);
	int res2 = linkedCellContainer.index(arr2);
	int res3 = linkedCellContainer.index(arr3);
	int res4 = linkedCellContainer.index(arr4);
	int res5 = linkedCellContainer.index(arr5);

	ASSERT_EQ(res1, 0);
	ASSERT_EQ(res2, 12);
	ASSERT_EQ(res3, 31);
	ASSERT_EQ(res4, 16);
	ASSERT_EQ(res5, 23);

	std::array<double, 3> dim1 = {7, 7, 1};
	std::array<int, 3> arr_1 = {3, 3, 0};

	linkedCellContainer = LinkedCellContainer{dim1[0], dim1[1], dim1[2], 1.};

	int res_1 = linkedCellContainer.index(arr_1);

	ASSERT_EQ(res_1, 24);
}

/**
 * This method is used to get a specific particle from the container
 * @param p the container
 * @param type the type of the particle
 * @return the first particle with the type
 */
Particle findParticle(const DirectSumParticleContainer &p, int type) {
	for (auto &e : p) {
		if (e.getType() == type) return e;
	}
	return Particle{};
}

/**
 * This method is used to get a specific particle from the container
 * @param p the container
 * @param type the type of the particle
 * @return the first particle with the type
 */
Particle findParticle(const LinkedCellContainer &p, int type) {
	for (auto &cell : p.getGrid()) {
		for (auto &e : cell) {
			if (e->getType() == type) return *e;
		}
	}
	return Particle{};
}


/**
 * Simple test to check whether the LinkedCellMethod works by comparing it with the lennard jones implementation
 * Populates the container with 4 particles each
 */
TEST(LinkedCellTest, LinkedCellMethodSimpleTest) {

	// The rounding error that we allow
	double EPSILON_VALUE = 0.01;
	double EPS = 5.;
	double SIGMA = 1.;

	DirectSumParticleContainer particleContainer = DirectSumParticleContainer{};
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{12, 12, 1, 3.};

	std::cout << "Grid size: " << linkedCellContainer.grid.size() << std::endl;

	// Populate the normal container and the linkedCell container
	for (int i = 0; i < 4; ++i) {
		particleContainer.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
		                               std::array<double, 3>{0., 0., 0.},
		                               static_cast<double>(i),
		                               i);
		linkedCellContainer.particles.emplace_back(std::array<double, 3>{0.1 + i, 0.2 + i, 0.3},
		                                           std::array<double, 3>{0., 0., 0.},
		                                           static_cast<double>(i),
		                                           i);
	}
	// Moves the Particles to the correct cell
	linkedCellContainer.setup();

	// calculate our result
	calculator::LennardJones lj{SIGMA, EPS};
	lj.calcF(particleContainer);

	calculator::LinkedCell lc{SIGMA, EPS, 3.};
	lc.setEpsilonTable({{EPS}});
	lc.setSigmaTable({{SIGMA}});

	lc.calcF(linkedCellContainer);

	for (int i = 0; i < 4; ++i) {
		auto p1 = findParticle(particleContainer, i);
		auto p2 = findParticle(linkedCellContainer, i);
		EXPECT_LE(fabs(p1.getF()[0] - p2.getF()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getF()[1] - p2.getF()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getF()[2] - p2.getF()[2]), EPSILON_VALUE);
	}
}


/**
 * Tests whether the setup method correctly puts the particles into the correct cells
 */
TEST(LinkedCellTest, SetupMethodTest) {
	/**********************Test with 1 particle**********************************/
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{16, 14, 13, 3.};

	linkedCellContainer.particles.emplace_back(std::array<double, 3>{10, 4, 5},
	                                           std::array<double, 3>{0., 0., 0.},
	                                           static_cast<double>(1),
	                                           0);

	linkedCellContainer.setup();

	EXPECT_EQ(linkedCellContainer.grid[0].getParticles().size(), 0);
	EXPECT_EQ(linkedCellContainer.grid[linkedCellContainer.index(std::array<int, 3>{3, 1, 1})]
		          .getParticles().size(), 1);

	/**********************Test with 3 particles**********************************/
	LinkedCellContainer linkedCellContainer1 = LinkedCellContainer{120, 80, 100, 3.};

	linkedCellContainer1.particles.emplace_back(std::array<double, 3>{2.1, 2.8, 5.6},
	                                            std::array<double, 3>{0.1, 0.1, 0.1},
	                                            .05,
	                                            7);
	linkedCellContainer1.particles.emplace_back(std::array<double, 3>{2.4, 3.2, 6.4},
	                                            std::array<double, 3>{0.1, 0.1, 0.1},
	                                            .05,
	                                            8);
	linkedCellContainer1.particles.emplace_back(std::array<double, 3>{2.7, 3.6, 7.2},
	                                            std::array<double, 3>{0.1, 0.1, 0.1},
	                                            .05,
	                                            9);

	linkedCellContainer1.setup();

	EXPECT_EQ(linkedCellContainer1.grid[0].getParticles().size(), 0);
	EXPECT_EQ(linkedCellContainer1.grid[linkedCellContainer1.index(std::array<int, 3>{0, 1, 2})]
		          .getParticles().size(), 2);
	EXPECT_EQ(linkedCellContainer1.grid[linkedCellContainer1.index(std::array<int, 3>{0, 0, 1})]
		          .getParticles().size(), 1);
}


/**
 * Tests whether the outflow and cyclic boundaries work
 */
TEST(LinkedCellTest, OutflowAndCyclicTest) {
	// Check whether outflow works
	LinkedCellContainer cellContainer = LinkedCellContainer{16, 14, 13, 3.};

	cellContainer.particles.emplace_back(std::array<double, 3>{-1, 2, 2},
	                                     std::array<double, 3>{0., 0., 0.},
	                                     static_cast<double>(1), 0);
	cellContainer.particles.emplace_back(std::array<double, 3>{1, -2, 2},
	                                     std::array<double, 3>{0., 0., 0.},
	                                     static_cast<double>(1),
	                                     0);
	cellContainer.particles.emplace_back(std::array<double, 3>{1, 2, -2},
	                                     std::array<double, 3>{0., 0., 0.},
	                                     static_cast<double>(1),
	                                     0);
	for (auto it = cellContainer.particles.begin(); it != cellContainer.particles.end(); ++it) {
		cellContainer.grid[0].emplace_back(&(*it));
	}

	ASSERT_EQ(cellContainer.grid[0].getParticles().size(), 3);
	calculator::LinkedCell::moveParticles(cellContainer);
	cellContainer.setup();
	EXPECT_EQ(cellContainer.grid[0].getParticles().size(), 0);

	// Check whether cyclic works
	LinkedCellContainer cellContainer1 = LinkedCellContainer{12, 12, 12, 3.,
	                                                         std::array<LinkedCellContainer::Border, 6>{
		                                                         LinkedCellContainer::periodic,
		                                                         LinkedCellContainer::periodic,
		                                                         LinkedCellContainer::periodic,
		                                                         LinkedCellContainer::periodic,
		                                                         LinkedCellContainer::periodic,
		                                                         LinkedCellContainer::periodic}};

	cellContainer1.particles.emplace_back(std::array<double, 3>{-1, 2, 2},
	                                      std::array<double, 3>{0., 0., 0.},
	                                      static_cast<double>(1), 0);
	cellContainer1.particles.emplace_back(std::array<double, 3>{1, -2, 2},
	                                      std::array<double, 3>{0., 0., 0.},
	                                      static_cast<double>(1),
	                                      0);
	cellContainer1.particles.emplace_back(std::array<double, 3>{1, 2, -2},
	                                      std::array<double, 3>{0., 0., 0.},
	                                      static_cast<double>(1),
	                                      0);
	for (auto it = cellContainer1.particles.begin(); it != cellContainer1.particles.end(); ++it) {
		cellContainer1.grid[0].emplace_back(&(*it));
	}

	ASSERT_EQ(cellContainer1.grid[0].getParticles().size(), 3);
	calculator::LinkedCell::moveParticles(cellContainer1);
	cellContainer1.setup();
	EXPECT_EQ(cellContainer1.grid[cellContainer1.index(std::array<int, 3>{3, 0, 0})].getParticles().size(), 1);
	EXPECT_EQ(cellContainer1.grid[cellContainer1.index(std::array<int, 3>{0, 3, 0})].getParticles().size(), 1);
	EXPECT_EQ(cellContainer1.grid[cellContainer1.index(std::array<int, 3>{0, 0, 3})].getParticles().size(), 1);
}

/**
 * Tests reflective boundary with a single particle
 */
TEST(LinkedCellTest, ReflectiveBoundaryTest) {
	/**********************Test with 1 particle**********************************/
	double SIGMA = 1;
	// create domain with 3,3,3
	LinkedCellContainer linkedCellContainer = {3, 3, 3, 1., std::array<LinkedCellContainer::Border, 6>{
		LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective,
		LinkedCellContainer::reflective, LinkedCellContainer::reflective, LinkedCellContainer::reflective}};
	// set particle on the effectRadius;
	Particle particle
		{std::array<double, 3>{3 - (std::pow(2, 1.0 / 6.0) * SIGMA) + 0.2, 1.5, 1.5}, std::array<double, 3>{1, 0., 0.},
		 static_cast<double>(1)};
	linkedCellContainer.particles.emplace_back(particle);

	for (auto it = linkedCellContainer.particles.begin(); it != linkedCellContainer.particles.end(); ++it) {
		linkedCellContainer.grid[0].emplace_back(&(*it));
	}
	// one particle should be added
	ASSERT_EQ(linkedCellContainer.grid[0].getParticles().size(), 1);
	linkedCellContainer.setup();
	ASSERT_EQ(linkedCellContainer.grid[0].getParticles().size(), 0);
	// it should only move towards positive x
	EXPECT_EQ(linkedCellContainer.begin()->getV()[0], 1);
	EXPECT_EQ(linkedCellContainer.begin()->getV()[1], 0);
	EXPECT_EQ(linkedCellContainer.begin()->getV()[2], 0);
	// calculate next step
	calculator::LinkedCell calc{SIGMA, 1, 1};
	calc.setEpsilonTable({{1}});
	calc.setSigmaTable({{1}});
	calc.setDeltaT(0.00005);
	calc.calcX(linkedCellContainer);
	linkedCellContainer.setup();
	calc.calcF(linkedCellContainer);
	calc.calcV(linkedCellContainer);

	// it should slow down after entering effectRadius
	EXPECT_LT(linkedCellContainer.begin()->getV()[0], 1);
	EXPECT_EQ(linkedCellContainer.begin()->getV()[1], 0);
	EXPECT_EQ(linkedCellContainer.begin()->getV()[2], 0);
}

/**
 * Tests the LinkedCell Method more detailedly against the Lennard Jones implementation
 * Creates two container with 10 particles each and compares the values
*/
TEST(LinkedCellTest, LinkedCellMethodIntermediateTest) {
	// The rounding error that we allow
	double EPSILON_VALUE = 0.009;
	double EPS = 5.;
	double SIGMA = 1.;
	double delta_t = 0.0005;
	double rCut = 3.0;

	DirectSumParticleContainer particleContainer = DirectSumParticleContainer{};
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{120, 80, 100, rCut};

	std::cout << "Grid size: " << linkedCellContainer.grid.size() << std::endl;

	for (int i = 0; i < 10; ++i) {
		particleContainer.emplace_back(std::array<double, 3>{(0.1 * i) + 0.8 * i, (0.2 * i) + 0.8 * i,
		                                                     (0.6 * i) + 0.8 * i},
		                               std::array<double, 3>{0.1, 0.1, 0.1},
		                               .05,
		                               i);
		linkedCellContainer.particles.emplace_back(std::array<double, 3>{(0.1 * i) + 0.8 * i, (0.2 * i) + 0.8 * i,
		                                                                 (0.6 * i) + 0.8 * i},
		                                           std::array<double, 3>{0.1, 0.1, 0.1},
		                                           .05,
		                                           i);
	}

	linkedCellContainer.setup();

	// calculate our result
	calculator::LennardJones lj{SIGMA, EPS};
	lj.setDim(3);
	lj.setDeltaT(delta_t);

	lj.calcF(particleContainer);

	calculator::LinkedCell lc{SIGMA, EPS, rCut};
	lc.setEpsilonTable({{EPS}});
	lc.setSigmaTable({{SIGMA}});
	lc.setDim(3);
	lc.setDeltaT(delta_t);
	lc.calcF(linkedCellContainer);

	for (int i = 0; i < 10; ++i) {
		auto p1 = findParticle(particleContainer, i);
		auto p2 = findParticle(linkedCellContainer, i);
		std::cout << "F1 of P1 is: " << p1.getF()[0] << " F1 of P2 is: " << p2.getF()[0] << std::endl;
		std::cout << "F2 of P1 is: " << p1.getF()[1] << " F2 of P2 is: " << p2.getF()[1] << std::endl;
		std::cout << "F3 of P1 is: " << p1.getF()[2] << " F3 of P2 is: " << p2.getF()[2] << std::endl;
		EXPECT_LE(fabs(p1.getF()[0] - p2.getF()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getF()[1] - p2.getF()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getF()[2] - p2.getF()[2]), EPSILON_VALUE);
	}

	lj.calcX(particleContainer);
	lc.calcX(linkedCellContainer);

	for (int i = 0; i < 10; ++i) {
		auto p1 = findParticle(particleContainer, i);
		auto p2 = findParticle(linkedCellContainer, i);
		std::cout << "X of P1 is: " << p1.getX()[0] << " X of P2 is: " << p2.getX()[0] << std::endl;
		std::cout << "Y of P1 is: " << p1.getX()[1] << " Y of P2 is: " << p2.getX()[1] << std::endl;
		std::cout << "Z of P1 is: " << p1.getX()[2] << " Z of P2 is: " << p2.getX()[2] << std::endl;
		EXPECT_LE(fabs(p1.getX()[0] - p2.getX()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getX()[1] - p2.getX()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getX()[2] - p2.getX()[2]), EPSILON_VALUE);
	}

	lj.calcV(particleContainer);
	lc.calcV(linkedCellContainer);

	for (int i = 0; i < 10; ++i) {
		auto p1 = findParticle(particleContainer, i);
		auto p2 = findParticle(linkedCellContainer, i);
		std::cout << "V1 of P1 is: " << p1.getV()[0] << " V1 of P2 is: " << p2.getV()[0] << std::endl;
		std::cout << "V2 of P1 is: " << p1.getV()[1] << " V2 of P2 is: " << p2.getV()[1] << std::endl;
		std::cout << "V3 of P1 is: " << p1.getV()[2] << " V3 of P2 is: " << p2.getV()[2] << std::endl;
		EXPECT_LE(fabs(p1.getV()[0] - p2.getV()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getV()[1] - p2.getV()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getV()[2] - p2.getV()[2]), EPSILON_VALUE);
	}

	/************************************* Second Container Test *************************************/

	DirectSumParticleContainer particleContainer2 = DirectSumParticleContainer{};
	LinkedCellContainer linkedCellContainer2 = LinkedCellContainer{120, 80, 100, rCut};

	std::cout << "Grid size: " << linkedCellContainer2.grid.size() << std::endl;

	for (int i = 0; i < 10; ++i) {
		particleContainer2.emplace_back(std::array<double, 3>{(0.4 * i) + 0.8 * i, (0.6 * i) + 0.8 * i,
		                                                      (1.2 * i) + 0.8 * i},
		                                std::array<double, 3>{0.1, 0.1, 0.1},
		                                .05,
		                                i);
		linkedCellContainer2.particles.emplace_back(std::array<double, 3>{(0.4 * i) + 0.8 * i, (0.6 * i) + 0.8 * i,
		                                                                  (1.2 * i) + 0.8 * i},
		                                            std::array<double, 3>{0.1, 0.1, 0.1},
		                                            .05,
		                                            i);
	}

	linkedCellContainer2.setup();

	// calculate our result
	calculator::LennardJones lj2{SIGMA, EPS};
	lj2.setDim(3);
	lj2.setDeltaT(delta_t);

	lj2.calcF(particleContainer2);

	calculator::LinkedCell lc2{SIGMA, EPS, rCut};
	lc2.setEpsilonTable({{EPS}});
	lc2.setSigmaTable({{SIGMA}});
	lc2.setDim(3);
	lc2.setDeltaT(delta_t);
	lc2.calcF(linkedCellContainer2);

	for (int i = 0; i < 10; ++i) {
		auto p1 = findParticle(particleContainer2, i);
		auto p2 = findParticle(linkedCellContainer2, i);
		std::cout << "F1 of P1 is: " << p1.getF()[0] << " F1 of P2 is: " << p2.getF()[0] << std::endl;
		std::cout << "F2 of P1 is: " << p1.getF()[1] << " F2 of P2 is: " << p2.getF()[1] << std::endl;
		std::cout << "F3 of P1 is: " << p1.getF()[2] << " F3 of P2 is: " << p2.getF()[2] << std::endl;
		EXPECT_LE(fabs(p1.getF()[0] - p2.getF()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getF()[1] - p2.getF()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getF()[2] - p2.getF()[2]), EPSILON_VALUE);
	}

	lj.calcX(particleContainer2);
	lc.calcX(linkedCellContainer2);

	for (int i = 0; i < 10; ++i) {
		auto p1 = findParticle(particleContainer2, i);
		auto p2 = findParticle(linkedCellContainer2, i);
		std::cout << "X of P1 is: " << p1.getX()[0] << " X of P2 is: " << p2.getX()[0] << std::endl;
		std::cout << "Y of P1 is: " << p1.getX()[1] << " Y of P2 is: " << p2.getX()[1] << std::endl;
		std::cout << "Z of P1 is: " << p1.getX()[2] << " Z of P2 is: " << p2.getX()[2] << std::endl;
		EXPECT_LE(fabs(p1.getX()[0] - p2.getX()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getX()[1] - p2.getX()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getX()[2] - p2.getX()[2]), EPSILON_VALUE);
	}

	lj.calcV(particleContainer2);
	lc.calcV(linkedCellContainer2);

	for (int i = 0; i < 10; ++i) {
		auto p1 = findParticle(particleContainer2, i);
		auto p2 = findParticle(linkedCellContainer2, i);
		std::cout << "V1 of P1 is: " << p1.getV()[0] << " V1 of P2 is: " << p2.getV()[0] << std::endl;
		std::cout << "V2 of P1 is: " << p1.getV()[1] << " V2 of P2 is: " << p2.getV()[1] << std::endl;
		std::cout << "V3 of P1 is: " << p1.getV()[2] << " V3 of P2 is: " << p2.getV()[2] << std::endl;
		EXPECT_LE(fabs(p1.getV()[0] - p2.getV()[0]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getV()[1] - p2.getV()[1]), EPSILON_VALUE);
		EXPECT_LE(fabs(p1.getV()[2] - p2.getV()[2]), EPSILON_VALUE);
	}
}

/**
 * Tests whether the periodic forces are correctly calculated in 2D
 */
TEST(LinkedCellTest, PeriodicBoundary2DTest) {
	double EPSILON_VALUE = 0.000000001;

	std::array<LinkedCellContainer::Border, 6> bor{};
	bor.fill(LinkedCellContainer::periodic);
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{5, 5, 1, 1., bor};

	Particle p1 = Particle{1};
	Particle p2 = Particle{2};
	Particle p3 = Particle{3};
	Particle p4 = Particle{4};

	p1.setX({1.5, 1.4, 0.});
	p2.setX({2.45, 1.6, 0.});
	p3.setX({0.5, 4.6, 0.});
	p4.setX({4.55, 4.4, 0.});

	linkedCellContainer.particles.emplace_back(p1);
	linkedCellContainer.particles.emplace_back(p2);
	linkedCellContainer.particles.emplace_back(p3);
	linkedCellContainer.particles.emplace_back(p4);

	linkedCellContainer.setup();

	calculator::LinkedCell lc = calculator::LinkedCell{1.2, 1.0, 1.};

	lc.calcF(linkedCellContainer);

	auto pa = findParticle(linkedCellContainer, 1);
	auto pb = findParticle(linkedCellContainer, 2);
	std::cout << "F1 of P1 is: " << pa.getF()[0] << " F1 of P2 is: " << pb.getF()[0] << std::endl;
	std::cout << "F2 of P1 is: " << pa.getF()[1] << " F2 of P2 is: " << pb.getF()[1] << std::endl;
	std::cout << "F3 of P1 is: " << pa.getF()[2] << " F3 of P2 is: " << pb.getF()[2] << std::endl;

	auto pc = findParticle(linkedCellContainer, 3);
	auto pd = findParticle(linkedCellContainer, 4);
	std::cout << "F1 of P1 is: " << pc.getF()[0] << " F1 of P2 is: " << pd.getF()[0] << std::endl;
	std::cout << "F2 of P1 is: " << pc.getF()[1] << " F2 of P2 is: " << pd.getF()[1] << std::endl;
	std::cout << "F3 of P1 is: " << pc.getF()[2] << " F3 of P2 is: " << pd.getF()[2] << std::endl;

	EXPECT_LE(fabs(pa.getF()[0] - pd.getF()[0]), EPSILON_VALUE);
	EXPECT_LE(fabs(pa.getF()[1] - pd.getF()[1]), EPSILON_VALUE);
	EXPECT_LE(fabs(pa.getF()[2] - pd.getF()[2]), EPSILON_VALUE);

	EXPECT_LE(fabs(pb.getF()[0] - pc.getF()[0]), EPSILON_VALUE);
	EXPECT_LE(fabs(pb.getF()[1] - pc.getF()[1]), EPSILON_VALUE);
	EXPECT_LE(fabs(pb.getF()[2] - pc.getF()[2]), EPSILON_VALUE);

}

/**
 * Tests whether the periodic forces are correctly calculated in 3D
 */
TEST(LinkedCellTest, PeriodicBoundary3DTest) {
	double EPSILON_VALUE = 0.000000001;

	std::array<LinkedCellContainer::Border, 6> bor{};
	bor.fill(LinkedCellContainer::periodic);
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{5, 5, 5, 1., bor};

	Particle p1 = Particle{1};
	Particle p2 = Particle{2};
	Particle p3 = Particle{3};
	Particle p4 = Particle{4};

	p1.setX({1.5, 1.4, 1.2});
	p2.setX({2.4, 1.6, 1.4});
	p3.setX({0.5, 4.6, 4.4});
	p4.setX({4.6, 4.4, 4.2});

	linkedCellContainer.particles.emplace_back(p1);
	linkedCellContainer.particles.emplace_back(p2);
	linkedCellContainer.particles.emplace_back(p3);
	linkedCellContainer.particles.emplace_back(p4);

	linkedCellContainer.setup();

	calculator::LinkedCell lc = calculator::LinkedCell{1.2, 1.0, 1.};

	lc.calcF(linkedCellContainer);

	auto pa = findParticle(linkedCellContainer, 1);
	auto pb = findParticle(linkedCellContainer, 2);
	std::cout << "F1 of P1 is: " << pa.getF()[0] << " F1 of P2 is: " << pb.getF()[0] << std::endl;
	std::cout << "F2 of P1 is: " << pa.getF()[1] << " F2 of P2 is: " << pb.getF()[1] << std::endl;
	std::cout << "F3 of P1 is: " << pa.getF()[2] << " F3 of P2 is: " << pb.getF()[2] << std::endl;

	auto pc = findParticle(linkedCellContainer, 3);
	auto pd = findParticle(linkedCellContainer, 4);
	std::cout << "F1 of P1 is: " << pc.getF()[0] << " F1 of P2 is: " << pd.getF()[0] << std::endl;
	std::cout << "F2 of P1 is: " << pc.getF()[1] << " F2 of P2 is: " << pd.getF()[1] << std::endl;
	std::cout << "F3 of P1 is: " << pc.getF()[2] << " F3 of P2 is: " << pd.getF()[2] << std::endl;

	EXPECT_LE(fabs(pa.getF()[0] - pd.getF()[0]), EPSILON_VALUE);
	EXPECT_LE(fabs(pa.getF()[1] - pd.getF()[1]), EPSILON_VALUE);
	EXPECT_LE(fabs(pa.getF()[2] - pd.getF()[2]), EPSILON_VALUE);

	EXPECT_LE(fabs(pb.getF()[0] - pc.getF()[0]), EPSILON_VALUE);
	EXPECT_LE(fabs(pb.getF()[1] - pc.getF()[1]), EPSILON_VALUE);
	EXPECT_LE(fabs(pb.getF()[2] - pc.getF()[2]), EPSILON_VALUE);
}

/**
 * Tests whether the particles go through the borders
 * One particle from top right to bottom left, and one from top right to bottom left
 */
TEST(LinkedCellTest, PeriodicBoundaryTransitionTest) {
	double EPSILON_VALUE = 0.000000001;

	std::array<LinkedCellContainer::Border, 6> bor{};
	bor.fill(LinkedCellContainer::periodic);
	LinkedCellContainer linkedCellContainer = LinkedCellContainer{5, 5, 1, 1., bor};

	Particle p1 = Particle{1};
	Particle p2 = Particle{2};

	p1.setX({0.0, 0.0, 0.0});
	p2.setX({4.99, 0.0, 0.0});

	p1.setF({-0.05, -0.05, 0.0});
	p2.setF({0.5, -0.5, 0.0});

	p1.setV({-0.05, -0.05, 0.0});
	p2.setV({0.5, -0.5, 0.0});

	linkedCellContainer.particles.emplace_back(p1);
	linkedCellContainer.particles.emplace_back(p2);

	linkedCellContainer.setup();

	calculator::LinkedCell lc = calculator::LinkedCell{1.2, 1.0, 1.};

	lc.calcX(linkedCellContainer);
	linkedCellContainer.setup();

	ASSERT_EQ(linkedCellContainer.grid[24].getParticles().size(), 1);
	ASSERT_EQ(linkedCellContainer.grid[20].getParticles().size(), 1);
}

#ifdef _OPENMP

class LinkedCellParallelTest : public testing::Test {
protected:
	LinkedCellContainer lcSerial;
	LinkedCellContainer lcPrimitiveX;
	LinkedCellContainer lcPrimitiveY;
	LinkedCellContainer lcPrimitiveZ;
	LinkedCellContainer lcSubDomain;

	double x = 70.;
	double y = 70.;
	double z = 70.;
	std::array<double, 3> g = std::array<double, 3>{0., -2., 0.};
	double rCut = 3.;

	// The rounding error that we allow
	double EPSILON_VALUE = 0.00000001;
	double EPS = 5.;
	double SIGMA = 1.;
	double delta_t = 0.0005;
	calculator::LinkedCell lc = calculator::LinkedCell{SIGMA, EPS, rCut};

	void SetUp(int threadNum) {
		omp_set_num_threads(threadNum);
		spdlog::set_level(spdlog::level::off);
		using lcb = LinkedCellContainer::Border;
		auto borders =
			std::array<LinkedCellContainer::Border, 6>{lcb::periodic, lcb::periodic, lcb::reflective, lcb::reflective,
			                                           lcb::periodic, lcb::periodic};

		lcSerial = LinkedCellContainer{x, y, z, rCut, borders, g, LinkedCellContainer::Strategy::serial};
		lcPrimitiveX = LinkedCellContainer{x, y, z, rCut, borders, g, LinkedCellContainer::Strategy::primitiveX};
		lcPrimitiveY = LinkedCellContainer{x, y, z, rCut, borders, g, LinkedCellContainer::Strategy::primitiveY};
		lcPrimitiveZ = LinkedCellContainer{x, y, z, rCut, borders, g, LinkedCellContainer::Strategy::primitiveZ};
		lcSubDomain = LinkedCellContainer{x, y, z, rCut, borders, g, LinkedCellContainer::Strategy::subDomain};

		lc.setEpsilonTable({{EPS}});
		lc.setSigmaTable({{SIGMA}});
		lc.setDim(3);
		lc.setDeltaT(delta_t);

		initializeSerial();
		initializePrimitiveX();
		initializePrimitiveY();
		initializePrimitiveZ();
		initializeSubDomain();
	}

	void initializeSerial() {
		initializeContainer(lcSerial);
		ASSERT_EQ(1000, lcSerial.size());
	}

	void initializePrimitiveX() {
		initializeContainer(lcPrimitiveX);
		ASSERT_EQ(1000, lcPrimitiveX.size());
	}

	void initializePrimitiveY() {
		initializeContainer(lcPrimitiveY);
		ASSERT_EQ(1000, lcPrimitiveY.size());
	}

	void initializePrimitiveZ() {
		initializeContainer(lcPrimitiveZ);
		ASSERT_EQ(1000, lcPrimitiveZ.size());
	}

	void initializeSubDomain() {
		initializeContainer(lcSubDomain);
		ASSERT_EQ(1000, lcSubDomain.size());
	}

	static void initializeContainer(ParticleContainer &particleContainer) {
		int i = 0;
		for (int x = 1; x <= 10; ++x) {
			for (int y = 1; y <= 10; ++y) {
				for (int z = 1; z <= 10; ++z) {
					double x_ = x * 1.2;
					double y_ = y * 1.2;
					double z_ = z * 1.2;
					particleContainer.emplace_back(std::array<double, 3>{x_, y_, z_},
					                               std::array<double, 3>{0., 0., 0.},
					                               .05,
					                               i);
					i++;
				}
			}
		}
		particleContainer.setup();
	}

	/**
	 * Simulates the first iteration
	 */
	void firstIteration() {
		lc.calcF(lcSerial);
		lc.calcF(lcPrimitiveX);
		lc.calcF(lcPrimitiveY);
		lc.calcF(lcPrimitiveZ);
		lc.calcF(lcSubDomain);
	}

	/**
	 * Simulates one iteration
	 */
	void oneIteration() {
		lc.calcX(lcSerial);
		lc.calcX(lcPrimitiveX);
		lc.calcX(lcPrimitiveY);
		lc.calcX(lcPrimitiveZ);
		lc.calcX(lcSubDomain);

		lcSerial.setup();
		lcPrimitiveX.setup();
		lcPrimitiveY.setup();
		lcPrimitiveZ.setup();
		lcSubDomain.setup();

		lc.calcF(lcSerial);
		lc.calcF(lcPrimitiveX);
		lc.calcF(lcPrimitiveY);
		lc.calcF(lcPrimitiveZ);
		lc.calcF(lcSubDomain);

		lc.calcV(lcSerial);
		lc.calcV(lcPrimitiveX);
		lc.calcV(lcPrimitiveY);
		lc.calcV(lcPrimitiveZ);
		lc.calcV(lcSubDomain);
	}

	/**
	 * Tests whether every particle has the same force values
	 */
	void testEquality() {
		for (int i = 0; i < 1000; ++i) {
			auto p_ref = findParticle(lcSerial, i);
			auto p_x = findParticle(lcPrimitiveX, i);
			auto p_y = findParticle(lcPrimitiveY, i);
			auto p_z = findParticle(lcPrimitiveZ, i);
			auto p_sd = findParticle(lcSubDomain, i);

			EXPECT_LE(fabs(p_ref.getF()[0] - p_x.getF()[0]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[1] - p_x.getF()[1]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[2] - p_x.getF()[2]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[0] - p_y.getF()[0]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[1] - p_y.getF()[1]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[2] - p_y.getF()[2]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[0] - p_z.getF()[0]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[1] - p_z.getF()[1]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[2] - p_z.getF()[2]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[0] - p_sd.getF()[0]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[1] - p_sd.getF()[1]), EPSILON_VALUE);
			EXPECT_LE(fabs(p_ref.getF()[2] - p_sd.getF()[2]), EPSILON_VALUE);
//			std::cout<<"F1 of P1 is: "<<p_ref.getF()[0]<<" F1 of P2 is: "<<p_sd.getF()[0]<<std::endl;
//			std::cout<<"F2 of P1 is: "<<p_ref.getF()[1]<<" F2 of P2 is: "<<p_sd.getF()[1]<<std::endl;
//			std::cout<<"F3 of P1 is: "<<p_ref.getF()[2]<<" F3 of P2 is: "<<p_sd.getF()[2]<<std::endl;
		}
	}
};

/**
 * Tests whether the parallelization techniques work
 * Compares parallelization technique to the serial technique over 69 iterations with 1000 particles
 * @warning this test needs more than 10s to finish
 */
TEST_F(LinkedCellParallelTest, parallelStrategyTest) {
	for (int j = 1; j <= 8; ++j) {
		LinkedCellParallelTest::SetUp(j);
		firstIteration();
		testEquality();

		for (int i = 0; i < 69; ++i) {
			oneIteration();
			testEquality();
		}
	}
}

#endif