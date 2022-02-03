#include <gtest/gtest.h>
#include <particleContainers/LinkedCellContainer.h>
#include <utils/ParticleGenerator.h>
#include <inputReader/XMLReader.h>

class SpecialParticleTest : public testing::Test {
protected:
	/* The ParticleContainer we use in this test */
	LinkedCellContainer p;

	void SetUp() override {
		XMLReader::XMLInfo info = XMLReader::readFile("../../input/files/testing/special_particle_test.xml");
		p = LinkedCellContainer{info.linkedCellSize[0], info.linkedCellSize[1], info.linkedCellSize[2], info.rCut};
		ParticleGenerator::generateParticles(p, info.generatorInfos);
	}

	static bool compare_double_arr(const std::array<double, 3> &arr1, const std::array<double, 3> &arr2) {
		double eps = 0.000000001;
		return fabs(arr1[0] - arr2[0]) < eps && fabs(arr1[1] - arr2[1]) < eps && fabs(arr1[2] - arr2[2]) < eps;
	}

};

TEST_F(SpecialParticleTest, SpecialParticles) {
	std::vector<Particle *> parts{};
	auto part_it = p.particles.begin() - 1;
	for (int i = 1; i < 6; i++) {
		part_it = std::find_if(++part_it, p.particles.end(), [](const Particle &part) {
		  return part.getM() > 1.1;
		});

		if (part_it == p.particles.end()) {
			std::cout << "didnt find special particle" << std::endl;
			ASSERT_TRUE(false);
		}
		parts.push_back(&(*part_it));
	}

	ASSERT_TRUE(compare_double_arr(parts[0]->getBaseForce(), std::array<double, 3>{0, 0, 0.8}));
	ASSERT_TRUE(compare_double_arr(parts[1]->getV(), std::array<double, 3>{0, 0.5, 0}));
	ASSERT_DOUBLE_EQ(parts[2]->getM(), 4.0);
	ASSERT_TRUE(parts[3]->immovable);
	ASSERT_TRUE(parts[4]->membrane);
	p.reset_base_force();
	ASSERT_TRUE(compare_double_arr(parts[0]->getBaseForce(), std::array<double, 3>{0, 0, 0}));
}