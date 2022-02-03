#include <gtest/gtest.h>
#include <set>
#include <iostream>

#include "particleContainers/DirectSumParticleContainer.h"
#include "utils/ParticleGenerator.h"

/**
 * Fixture test class
 * Before every TEST_F, the SetUp() override and TearDown() override test will be executed
 */
class ContainerTest : public testing::Test {
protected:
	/* The ParticleContainer we use in this test */
	DirectSumParticleContainer p;

	void SetUp() override {
		ContainerTest::init();
	}

	/**
	 * Constructs the container
	 */
	void init() {
		p = DirectSumParticleContainer{};
	}

	/**
	 * Generates a small input dataset (JSON file) in the ./input/files directory
	 * and reads it into the ParticleContainer
	 */
	void readSmallData() {
		if (std::system("python ../../input/generate_json.py --size small --output small.json")) {
			FAIL() << "There was a problem generating the input file in the tests!";
		}

		ParticleGenerator::generateParticles(p, "../../input/files/small.json");
	}

	/**
	 * Generates a medium input dataset (JSON file) in the ./input/files directory
	 * and reads it into the ParticleContainer
	 */

	void readMediumData() {
		if (std::system("python ../../input/generate_json.py --size medium --output medium.json")) {
			FAIL() << "There was a problem generating the input file in the tests!";
		}

		ParticleGenerator::generateParticles(p, "../../input/files/medium.json");
	}

	/*
	 * Generates a large input dataset (JSON file) in the ./input/files directory
	 * and reads it into the ParticleContainer
	 */
	void readLargeData() {
		if (std::system("python ../../input/generate_json.py --size large --output large.json")) {
			FAIL() << "There was a problem generating the input file in the tests!";
		}

		ParticleGenerator::generateParticles(p, "../../input/files/large.json");
	}
};

/**
 * Tests whether the default constructor does its job
 */
TEST_F(ContainerTest, DefaultConstructor) {
	EXPECT_EQ(0u, p.size());
}

/**
 * Tests whether the FileReader correctly parses the JSON and
 * whether the ParticleContainer has the correct size with a small input size
 * Then proceeds to delete the file
 */
TEST_F(ContainerTest, SmallDataSetSize) {
	ContainerTest::readSmallData();
	ASSERT_EQ(p.size(), 120);
	if (std::remove("../../input/files/small.json")) {
		FAIL() << "There was a problem deleting the generated (small dataset) JSON file";
	}
}

/**
 * Tests whether the FileReader correctly parses the JSON and
 * whether the ParticleContainer has the correct size with a medium input size
 * Then proceeds to delete the file
 */
TEST_F(ContainerTest, MediumDataSetSize) {
	ContainerTest::readMediumData();
	EXPECT_EQ(p.size(), 432);
	if (std::remove("../../input/files/medium.json")) {
		FAIL() << "There was a problem deleting the generated (medium dataset) JSON file";
	}
}

/**
 * Tests whether the FileReader correctly parses the JSON and
 * whether the ParticleContainer has the correct size with a large input size
 * Then proceeds to delete the file
 */
TEST_F(ContainerTest, LargeDataSetSize) {
	ContainerTest::readLargeData();
	EXPECT_EQ(p.size(), 768);
	if (std::remove("../../input/files/large.json")) {
		FAIL() << "There was a problem deleting the generated (large dataset) JSON file";
	}
}





