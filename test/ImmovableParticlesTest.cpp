#include <gtest/gtest.h>
#include <particleContainers/LinkedCellContainer.h>
#include <utils/ParticleGenerator.h>
#include <physicsCalculator/LinkedCell.h>
#include <inputReader/XMLReader.h>

class ImmovableParticlesTest : public testing::Test {
protected:
	/* The ParticleContainer we use in this test */
	LinkedCellContainer p;
	calculator::LinkedCell calc = calculator::LinkedCell{1., 1., 3., false, 0.};

	void SetUp() override {
		XMLReader::XMLInfo info = XMLReader::readFile("../../input/files/testing/immovable_particles_test.xml");
		p = LinkedCellContainer{info.linkedCellSize[0], info.linkedCellSize[1], info.linkedCellSize[2], info.rCut};
		calc = calculator::LinkedCell{info.sigma, info.epsilon, info.rCut, info.smoothed, info.rl};
		calc.setSigmaTable({{1, 1}, {1, 1}});
		calc.setEpsilonTable({{1, 1}, {1, 1}});
		ParticleGenerator::generateParticles(p, info.generatorInfos);
	}

	static bool compare_double_arr(const std::array<double, 3> &arr1, const std::array<double, 3> &arr2) {
		double eps = 0.000000001;
		return fabs(arr1[0] - arr2[0]) < eps && fabs(arr1[1] - arr2[1]) < eps && fabs(arr1[2] - arr2[2]) < eps;
	}

};

TEST_F(ImmovableParticlesTest, ImmovableParticles) {
	auto predicate = [](const Particle &p) {
	  if (p.immovable) { // immovable
		  return compare_double_arr(p.getF(), {0, 0, 0}) &&
			  compare_double_arr(p.getV(), {0, 0, 0}) &&
			  compare_double_arr(p.getX(), p.getOldX());

	  } else { // movable
		  return !compare_double_arr(p.getF(), {0, 0, 0}) &&
			  !compare_double_arr(p.getV(), {0, 0, 0}) &&
			  !compare_double_arr(p.getX(), p.getOldX());
	  }
	};

	// to check whether immovable particles moved
	for (auto &p : p) {
		p.setOldX({p.getX()});
	}

	p.setup();
	calc.calcF(p);
	for (int i = 0; i < 5; i++) {
		calc.calcX(p);
		p.setup();
		calc.calcF(p);
		calc.calcV(p);
	}
	ASSERT_TRUE(std::all_of(p.begin(), p.end(), predicate));
}